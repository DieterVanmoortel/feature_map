<?php
// Move this to a settings page.. 
define("BRANCHES_MINIMUM_RESULTSET_COUNT", 7);
define("BRANCHES_MAXIMUM_RESULTSET_COUNT", 10);
define("BRANCHES_MAXIMUM_DISTANCE", 40000);  // = 40km


/*
 * Callback for /map
 * base map display
 */
function feature_map_page() {
  global $language; 

  $element = array(
      '#tag' => 'meta',
      '#attributes' => array(
          'name' => 'viewport',
          'content' => 'initial-scale=1.0, user-scalable=no',
      ),
  );

  drupal_add_html_head($element, 'feature_map');
  $path = drupal_get_path('module', 'nas') . '/helpers/bpost/';
    
    // Send identifiers to JS
    drupal_add_js(array(
        'bpost' => array(
//            'identifiers' => $config['element_identifiers'],
            'process_path' => drupal_get_path('module', 'feature_map') . '/bpost/bpost.php',
            'language' => $language->language,
            )
        ), 'setting');   
    
  // Load required javascript  
  
  
  // we can download following on cron to the sites/default files : BIG TODO !
  drupal_add_js('http://maps.googleapis.com/maps/api/js?libraries=places&sensor=false&language=' . $language->language . '&region=be');
//  drupal_add_js('http://google-maps-utility-library-v3.googlecode.com/svn/tags/markerclustererplus/2.0.1/src/markerclusterer_packed.js');
//  drupal_add_js('https://raw.github.com/cowboy/jquery-bbq/v1.2.1/jquery.ba-bbq.min.js');
  drupal_add_js(drupal_get_path('module', 'feature_map') . '/js/jquery.ba-bbq.min.js');
  drupal_add_js(drupal_get_path('module', 'feature_map') . '/js/markerclusterer_packed.js');
  drupal_add_js(drupal_get_path('module', 'feature_map') . '/bpost/jquery.autocomplete.min.js');
  drupal_add_js(drupal_get_path('module', 'feature_map') . '/bpost/bpost.js');
//  drupal_add_js(drupal_get_path('module', 'feature_map') . '/js/infobubble.js');
  drupal_add_js(drupal_get_path('module', 'feature_map') . '/js/jquery.hoverintent.js');
  drupal_add_js(drupal_get_path('module', 'feature_map') . '/js/locator.js');
  
  
  // need to move this to a settings page too.
  drupal_add_js('var language="' . $language->language . '";
        var gmapsettings = {
            min_result_count:7,
            max_result_count:10,
            max_zoom:7,
            min_zoom_for_min_result:8,
            default_zoom:13,
            agent_infowindow_zoom:14,
            tab_info_title: "' . t('Information') .'",
            agent_json_url:"/ajax/locator",
            dummy:"" }
            ', array('type' => 'inline'));


  // add some styling
  drupal_add_css(drupal_get_path('module', 'nas') . '/helpers/bpost/jquery.autocomplete.css');
  drupal_add_css(drupal_get_path('module', 'feature_map') . '/css/feature_map.css');
  drupal_add_css(drupal_get_path('module', 'feature_map') . '/bpost/jquery.autocomplete.css');


  // the actual output
  $output = '';
  $output .= '<div id="map_wrapper" class="clear-block">';
  $output .= '<div id="map_canvas">' . t('Unable to load map') . '</div>';
  $output .= '<div id="map_options">';
  $output .= theme('map_description');
  $output .= theme('map_location');
  $output .= theme('map_filters');
  $output .= '</div>';
  $output .= '</div>';

  return $output;
}

/**
 * Fetch marker icon
 * @param type $node
 */
function feature_branches_marker_icon($node) {
  $default = 'rb_agent.png';
  $filepath = 'rb_';

  if (!empty($node->field_branch_broker) && isset($node->field_branch_broker[$node->language][0]['value'])) {
    if ($node->field_branch_broker[$node->language][0]['value'] == 1) {
      $filepath .= 'agent';
    }
    else {
      $filepath .= 'broker';
    }
  }
  else {
    return $default;
  }

  // Thanks to our designer for great naming conventions
  $sub = '';
  if (!empty($node->field_branch_atm) && isset($node->field_branch_atm[$node->language][0]['value'])) {
    if ($node->field_branch_atm[$node->language][0]['value'] > 0) {
      $sub .= '_atm';
    }
  }

  if (!empty($node->field_branch_selfrecord) && isset($node->field_branch_selfrecord[$node->language][0]['value'])) {
    if ($node->field_branch_selfrecord[$node->language][0]['value']) {
      if (empty($sub)) {
        $sub .= '_sr';
      }
      else {
        $sub .= 'sr';
      }
    }
  }

  // TYPE + ATM + SELFRECORD
  return $filepath . $sub . '.png';
}

function _frb_get_center($vars){
  return array(
    'lat' => ($vars['lat_2'] - $vars['lat_1']) / 2 + $vars['lat_1'],
    'lon' => ($vars['lon_2'] - $vars['lon_1']) / 2 + $vars['lon_1'],
  );
}

function sort_by_distance($a, $b) {
  if($a['distance'] > $b['distance']){
    return 1;
  }
  else return -1;
}