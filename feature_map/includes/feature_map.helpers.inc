<?php
// better add this to the variables
define('MAXIMUM_RESULT_COUNT', 10);
define('BRANCHES_MAXIMUM_DISTANCE', 40);


/**
 * 
 */
function map_view_type_options($variable, $options = array()){
  return array(
      'ROADMAP' => t('Roadmap'),
      'TERRRAIN' => t('Terrain'),
      'SATELLITE' => t('Satellite'),
  );
}
/**
 * 
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
      if($display){
        foreach($grid as $placemark){
          $items[$entity->title][] = $placemark;
        }
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
  $url = "http://maps.google.com/maps/geo?q=" . htmlentities($address);
  $url .= "&amp;output=xml";
  $ret = drupal_http_request($url);
  $result = json_decode($ret->data);
  if($result->Status->code == 200){
    if($display){
      // use this function to display a list of found lat/lon or multiple possibilities?
      return $result->Placemark;
    }
    else{
      if(count($result->Placemark) > 1){
        watchdog('Google Maps', t('Multiple locations found for '. l($url, $url)));
      }
      else{
        $position = new stdClass();
        $position->lon = $result->Placemark[0]->Point->coordinates[0];
        $position->lat = $result->Placemark[0]->Point->coordinates[1];
        return $position;
      }
    }
  }
  else{
    watchdog('Google Maps', t('Could not retrieve date from '. l($url, $url)));
    return FALSE;
  }
}

/**
 * Function that returns agent ids.
 * @param type $vars
 * @return array of id's
 */
function feature_map_get_markers($vars){

  if(intval($vars['postal'])) {
    //// only used the postal code here. Street & number can be added if desired.
    $address = intval($vars['postal']);
    $agent_ids = feature_form_search_postal($vars);
    $agents = node_load_multiple($agent_ids);
    order_by_ranking(&$agents);
    if(count($agents) < MAXIMUM_RESULT_COUNT) {
      $url = "http://maps.google.com/maps/geo?q=" . $address . ",belgium&amp;output=json";
      $ret = drupal_http_request($url);
      $data = $ret->data;
      $result = json_decode($data);
      $lon = $result->Placemark[0]->Point->coordinates[0];
      $lat = $result->Placemark[0]->Point->coordinates[1];
      $more = feature_form_search_proximity($lat, $lon, $agent_ids);
      order_by_distance($more, $lat, $lon);
      // reducing the number of results shown to the minimum result count
      $more = array_slice($more, 0, MAXIMUM_RESULT_COUNT - count($agent_ids), TRUE);
      $adding_agents = node_load_multiple(array_keys($more));
      // not using an array_merge to keep our key assoc.
      foreach($adding_agents as $nid => $node) {
        $agents[$nid] = $node;
        $agents[$nid]->distance = $more[$nid];
      }
    }

    foreach((array)$agents as $nid => $agent){
      $output[$nid] = '<h3 class="agent-title">'. $agent->title . '</h3>';
      $fields = array('str', 'zip', 'city');
      foreach($fields as $field){
        $fld = field_get_items('node', $agent, 'field_branch_address_' . $field);
        $output[$nid] .= '<span class="info-' .$field. '">' . $fld[0]['value'] . '</span>';
      }
      $output[$nid] .= l(t('More info'), 'node/' . $nid, array('attributes' => array('class' => 'info-more')));
      isset($agent->distance) ? $output[$nid] .= '<span class="info-distance">'. t('Distance:') . $agent->distance . ' km</span>' : '';
    }
    return $output;
  }
  else{
    watchdog('feature_form', 'The entered postal code was not a valid integer.');
  }
}

/**
 *  Function that returns agent ids with the given postal code
 * @param type $vars
 * @return array of id's
 */
function feature_map_search_postal($vars){
  // get all branches in the same postal code
//  $query = db_select('node', 'n');
//    $query->leftjoin('field_data_field_branch_address_zip', 'zip', 'n.nid = zip.entity_id');
//    $query->condition('field_branch_address_zip_value', $vars['postal']);
//    $query->condition('type', 'branch');
//    $query->condition('status', 1);
//    $query->fields('n', array('nid'));
//    $result = $query->execute()->fetchCol();

    return $result;
}
/**
 *  Function that returns agent ids within a given max range of the location
 * @param type $vars
 * @return array of id's
 */
function feature_map_search_proximity($lat, $lon, $excluded = NULL) {
  // not superclean, but lat +- 0.10 & lon +- 0.10 gives a radius of about 20km.

  // need to find a cleaner way..
  $north = $lat + 0.20;
  $south = $lat - 0.20;
  $east = $lon + 0.10;
  $west = $lon - 0.10;
  $query = db_select('node', 'n');
  // get results from map_coordinates table
  $query->leftjoin('map_coordinates', 'grid', 'n.nid = grid.entity_id');
  $query->condition('grid.lat', array($south, $north), 'BETWEEN');
  $query->condition('grid.lon', array($west, $east), 'BETWEEN');
  if($excluded){
    $query->condition('n.nid', $excluded, 'NOT IN');
  }
  // get this from settings page?
  $query->condition('type', 'branch');
  $query->condition('status', 1);
  $query->fields('n', array('nid'));
  $query->fields('grid', array('lat', 'lon'));
  $result = $query->execute()->fetchAllAssoc('nid');

  return $result;
}


/**
 * Order the agents by distance to target
 */
function order_by_distance(&$agents, $lat, $lon) {
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

}

/**
 *  Order agents by ranking (db)
 * @param type $agents
 */
function order_by_ranking(&$agents) {
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
}

/*
 * Mathematical calculation of a distance between two positions in km.
 * with $km = FALSE distance will be in 
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

function map_selected_countries($value = FALSE){
//  $countries = country_get_list();
  $t = get_t();
  $countries = array(
    'AT' => $t('Austria'), 
    'BE' => $t('Belgium'), 
    'CH' => $t('Switzerland'), 
    'DE' => $t('Germany'), 
    'DK' => $t('Denmark'), 
    'ES' => $t('Spain'), 
    'FI' => $t('Finland'), 
    'FR' => $t('France'), 
    'GB' => $t('United Kingdom'), 
    'GR' => $t('Greece'), 
    'IT' => $t('Italy'), 
    'LU' => $t('Luxembourg'), 
    'NL' => $t('Netherlands'), 
    'NO' => $t('Norway'), 
    'PL' => $t('Poland'), 
    'PT' => $t('Portugal'), 
    'SE' => $t('Sweden'), 
    'US' => $t('United States'), 
  );
  if($value){
    return $countries[$value];
  }
  // Sort the list.
  natcasesort($countries);

  return $countries;
}