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
  add_map_basics();


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

/*
 * Add necessary js & css for maps to show
 */
function add_map_basics(){
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