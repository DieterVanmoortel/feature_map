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

  return $items;
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



/*
 * Theme callback for the user location form block
 */
function theme_map_location($vars) {
  $output = '';
  $output .= '<div id="zipcodesearchform">';
  $output .= '<label for="edit-search">' . t('Enter your postal code, or a street and city') .'</label>';
  $output .= '<input class="form-text" type="text" id="location" value="" size="60" maxlength="128" />';
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
  $output .= '<h3>' . t('Replace by an actual form? ') . '</h3>';

  $options = array('option1', 'option2');
  $items = array();

  foreach ((array) $options as $option) {
    $items[] = '<label><input name="category" id="category_' . $option . '" value="' . $option . '" type="checkbox"><span class="title" title="' . t('[' . $option . ' description]') . '">' . t($option) . '</span></label>';
  }

  $output .= theme('item_list', array('items' => $items, 'attributes' => array('class' => array('filter-selection'))));

  $output .= '</div>';

  return $output;
}