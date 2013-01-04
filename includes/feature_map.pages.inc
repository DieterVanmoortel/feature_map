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
  global $language, $base_url;
  add_map_basics();

  $http_protocol = substr($base_url, 0, 5) == 'https' ? 'https' : 'http';
  drupal_add_js($http_protocol . '://google-maps-utility-library-v3.googlecode.com/svn/trunk/infobubble/src/infobubble-compiled.js ','external');

  drupal_add_js(array(
    'featureMap' => array(
        'ajaxPath' => base_path() . 'ajax/locator',
        'modulePath' => drupal_get_path('module', 'feature_map'),
        )
    ), 'setting');


  // the actual output
  $output = '';
  $output .= '<div id="map_wrapper" class="clear-block">';
  $output .= '<div id="map_canvas">' . t('Loading map') . '...</div>';
  $output .= theme('map_location');
  $output .= '<ul id="map_listing"></ul>';
  $output .= '<div id="map_options">';
//  $output .= theme('map_description');
  
//  $output .= theme('map_filters');
  $output .= '</div>';
  $output .= '</div>';

  return $output;
}
