<?php
// better add this to the variables

/*
 * Submit handler
 */
function feature_map_rebuild_menu(){
  if(menu_rebuild()){drupal_set_message(t('Menu rebuilt'));}
}
/**
 * select list callback : map types
 */
function map_view_type_options($variable, $options = array()){
  return array(
      'ROADMAP' => t('Roadmap'),
      'TERRRAIN' => t('Terrain'),
      'SATELLITE' => t('Satellite'),
  );
}
/**
 * select list callback : content types
 */
function map_view_content_types() {
  foreach(entity_get_info() as $type => $value) {
    if($value['bundles']) {
      foreach($value['bundles'] as $bundle => $item) {
        $types[$bundle] = $item['label'];
      }     
    }
  }
  return $types;
}
/**
 * Get coordinates for an address
 * @param type $address : array
 * @param type $entity : object
 * @param type $display : boolean
 */
function retrieve_map_coordinates($address, $entity, $display = FALSE){
      $countrycheck = variable_get($entity->type .'_mapping_country_check', FALSE);      
      $countrycheck ? $country = variable_get($entity->type .'_mapping_country', FALSE) : '';
      $countrycheck ? $address[] = map_selected_countries($country) : '';
      $address = implode(',',$address);
      // fetch from google maps
      $grid = map_fetch_grid($address, $display);
      dpm($grid);
      if($grid && $display){
        foreach($grid as $placemark){
          $items[$entity->vid][] = $placemark;
        }
        $items[$entity->vid]['title'] = $entity->title;
        $items[$entity->vid]['vid'] = $entity->vid;
        return $items;
      }
      elseif($grid){
        // write coordinates to db
        db_merge('map_coordinates')
          ->key(array('entity_id' => $entity->vid))
          ->fields(array(
              'lat' => $grid->lat,
              'lon' => $grid->lon,
          ))
          ->execute();
      }
    }
/**
 *  Retrieves lat & lon from google maps api
 * @param type $address a string
 * @return stdClass 
 */
function map_fetch_grid($address, $display = FALSE){
  // todo; check if we don't need an urlencode or htmlentities around $address
  $url = "http://maps.googleapis.com/maps/api/geocode/json?address=";
  $url .= $address;
  $url .= "&sensor=false";

  $ret = drupal_http_request($url);
  dpm($ret);
  if ($ret->code == 200) {
    $data = json_decode($ret->data);
    if ($display) {
      // use this function to display a list of found lat/lon or multiple possibilities?
      dpm('needs work!!! ');
      return 'work to be done';
    }
    else{
      if(count($data->results) > 1){
        watchdog('Google Maps', t('Multiple locations found for '. l($url, $url)));
      }
      else{
        $result = $data->results[0];
        $position = new stdClass();
        $position->lon = $result->geometry->location->lng;
        $position->lat = $result->geometry->location->lat;
        return $position;
      }
    }
  }
  else{
    watchdog('Google Maps', t('Could not retrieve data from '. l($url, $url)));
    return FALSE;
  }
}


/**
 * Order the agents by distance to target
 */
function order_by_distance($agents, $lat, $lon) {
  foreach($agents as $nid => $agent) {
    // can't use Google maps api because of Query limits..
    // http://maps.googleapis.com/maps/api/distancematrix/json?origins=1000,belgium,belgium&destinations=50.797490,4.424360&mode=driving&sensor=false

    // need to get grids from map coordinates table.
    $dist = math_distance($lat, $lon, $agent->field_branch_google_lat_value, $agent->field_branch_google_lon_value);
    // this is a var!! 
    if($dist < BRANCHES_MAXIMUM_DISTANCE){
        $nodes[$agent->nid] = $dist;
      }
  }
  asort($nodes);
  $agents = $nodes;
  return $agents;

}

/**
 *  Order agents by ranking (db)
 * @param type $agents
 */
function order_by_ranking($agents) {
  foreach($agents as $nid => $agent) {
    $rankingvalue = field_get_items('node', $agent, 'field_branch_ranking_banking');
    $rankingvalue = $rankingvalue[0]['value'];
    if(!empty($rankingvalue)){
      $ranking[$nid] = $rankingvalue;
    }
    else {$ranking[$nid] = 1000;}

  }
  asort($ranking);
  foreach ((array)$ranking as $nid => $item) {
      $agentset[$nid] = $agents[$nid];
      unset($agents[$nid]);
    }
  $agents = $agentset;
  return $agents;
}

function map_selected_countries($value = FALSE){
//  $countries = country_get_list();
  $t = get_t();
  $countries = array(
//    'AT' => $t('Austria'), 
    'BE' => $t('Belgium'), 
//    'CH' => $t('Switzerland'), 
    'DE' => $t('Germany'), 
//    'DK' => $t('Denmark'), 
//    'ES' => $t('Spain'), 
//    'FI' => $t('Finland'), 
    'FR' => $t('France'), 
    'GB' => $t('United Kingdom'), 
//    'GR' => $t('Greece'), 
//    'IT' => $t('Italy'), 
    'LU' => $t('Luxembourg'), 
    'NL' => $t('Netherlands'), 
//    'NO' => $t('Norway'), 
//    'PL' => $t('Poland'), 
//    'PT' => $t('Portugal'), 
//    'SE' => $t('Sweden'), 
//    'US' => $t('United States'), 
  );
  if($value){
    return $countries[$value];
  }
  // Sort the list.
  natcasesort($countries);

  return $countries;
}

/*
 * Helper : calculate distance using coordinates
 * Try to fetch from Google Maps or use a mathematical equation
 */
function calculate_distance($lat1, $lon1, $lat2, $lon2, $km = TRUE) {
  // try to get distance from Google MAPS
  if (isset($lat1, $lon1, $lat2, $lon2)) {
    $url = 'http://maps.google.com/maps/nav?output=json&q=from:'.$lat1 . ','. $lon2 . '%20to:' . $lat2 . ',' . $lon2;
    $result = file_get_contents($url);
    $data = json_decode(utf8_encode($result), true);
    if($data['Status']['code'] == 200) {
      // distance in km
      $distance = $data['Directions']['Distance']['meters'] / 1000;
    }
    else{
      $distance = math_distance($lat1, $lon1, $lat2, $lon2, $km);
    }
    return number_format($distance,1);
  }
}

/*
 * Mathematical calculation of a distance between two positions in km.
 * with $km = FALSE distance will be in miles
*/
function math_distance($lat1, $lon1, $lat2, $lon2, $km = TRUE) {
  $theta = $lon1 - $lon2;
  $dist = sin(deg2rad($lat1)) * sin(deg2rad($lat2)) +  cos(deg2rad($lat1)) * cos(deg2rad($lat2)) * cos(deg2rad($theta));
  $dist = acos($dist);
  $dist = rad2deg($dist);
  $distance = $dist * 60 * 1.1515;
  $km ? $distance = $distance * 1.609344 : '';
  return intval( $distance * 10) / 10;
}

function sort_by_distance($a, $b) {
  if($a['distance'] > $b['distance']){
    return 1;
  }
  else return -1;
}

/*
 * Add necessary js & css for maps to show
 */
function add_map_basics() {
  global $language, $base_url;
  $element = array(
      '#tag' => 'meta',
      '#attributes' => array(
          'name' => 'viewport',
          'content' => 'initial-scale=1.0, user-scalable=no',
      ),
  );
  drupal_add_html_head($element, 'feature_map');

  $http_protocol = substr($base_url, 0, 5) == 'https' ? 'https' : 'http';

  drupal_add_js($http_protocol . '://maps.googleapis.com/maps/api/js?libraries=places&sensor=false&language=' . $language->language, 'external');
  drupal_add_js($http_protocol . '://google-maps-utility-library-v3.googlecode.com/svn/tags/markerclustererplus/2.0.1/src/markerclusterer_packed.js', 'external');
  drupal_add_js(drupal_get_path('module', 'feature_map') . '/js/locator.js');

  // add some styling
  drupal_add_css(drupal_get_path('module', 'feature_map') . '/css/feature_map.css');
}