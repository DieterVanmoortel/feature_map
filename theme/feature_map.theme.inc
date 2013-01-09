<?php
/**
 * Implements hook_theme().
 */
function feature_map_theme($existing, $type, $theme, $path) {
  $items = array();

  $defaults = array(
      'path' => drupal_get_path('module', 'feature_map') . '/theme',
      'file' => 'feature_map.theme.inc',
  );

  $items['map_filters'] = array(
      'arguments' => array('node' => NULL)
      ) + $defaults;

  $items['map_description'] = array(
      'arguments' => array('node' => NULL),
  ) + $defaults;

  $items['map_location'] = array(
      'arguments' => array('node' => NULL  ),
  ) + $defaults;

  $items['mapview'] = array(
      'arguments' => array('marker' => NULL  ),
  ) + $defaults;
  
  $items['map_timeline'] = array(
      'arguments' => array('markers' => NULL ),
  ) + $defaults;

  $items['entity_preview'] = array(
      'arguments' => array(
        'lat' => NULL,
        'lon' => NULL
        ),
  ) + $defaults;

  return $items;
}

function theme_entity_preview($lat, $lon){
  add_map_basics();
  drupal_add_js(array(
    'feature_map' => array(
        'lat' => $lat,
        'lng' => $lon,
    )
  ), 'setting');

  $out = '<div id="preview">';
  $out .= '<div id="gmap">Loading map...</div>';
  $out .= '</div>';
  return $out;
}

function theme_listing($data) {
    // Q: use display suite or make a custom listing field?

  return $data;
}
/*
 * Theme callback for the description block
 */
function theme_map_description($vars){
  $output = '<div id="descriptionblock">';
  $output .= 'Marker description';
  $output .= '</div>';
  return $output;
}

function theme_map_timeline($vars) {
  $out = '<div id="map_timeline">';
  $out .= '<h3>' . t('Months') . '</h3>';
  for ($i=1; $i<=12; $i++){
    $months[] = '<span class="month-' . $i . '">'. map_month($i) . '</span>';
  }
  $out .= theme('item_list', array('items' => $months));
  $out .= '</div>';
  return $out;
}


/*
 * Theme callback for the user location form block
 */
function theme_map_location($vars) {
  $output = '';
  $output .= '<div id="zipcodesearchform">';
  $output .= '<label for="edit-search">' . t('Enter your postal code, or a street and city') .'</label>';
  $output .= '<input class="form-text" type="text" id="location" value="" maxlength="128" />';
  $output .= '<input class="location-submit" type="submit" value="' . t('Search') . '" />';
  $output .= '</div>';
  return $output;
}



/*
 * Theme callback for the filters block
 */
function theme_map_filters() {
  $output = "";
  $output .= '<h5>Distance slider!</h5>';
  $output .= '<div class="padded"><p><label for="amount">Distance:</label>';
  $output .= '<input type="text" id="amount" style="border:0; color:#f6931f; font-weight:bold;" />';
  $output .= '</p><div id="slider-range"></div></div>';
  $output .= '<div id="categories" class="clear-block">';
  $output .= '<h3>' . t('Filter results') . '</h3>';

  $output .= '</div>';

  return $output;
}