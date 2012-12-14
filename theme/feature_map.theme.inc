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
      'arguments' => array('node' => NULL),
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
  $output .= '<input class="bpost_autocomplete form-text ac_input" type="text" id="location" name="zipcodesearch" value="" size="60" maxlength="128" />';
  $output .= '<input class="trigger-lookup form-submit" type="submit" id="locate" name="lookup" value="' . t('Search') . '" />';
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

/**
 * Block: Renders a google map with agent details.
 */
function theme_location_summary() {
  global $language;

  if (arg(0) == 'node' && is_numeric(arg(1))) {
    $node = node_load(arg(1));
    if ($node->type == 'branch' && !empty($node->field_branch_google_lon) && isset($node->field_branch_google_lon[$node->language][0]['safe_value']) && !empty($node->field_branch_google_lat) && isset($node->field_branch_google_lat[$node->language][0]['safe_value'])) {

      drupal_add_js('http://maps.googleapis.com/maps/api/js?libraries=places&sensor=false&language=' . $language->language . '&region=be');
      drupal_add_js('http://google-maps-utility-library-v3.googlecode.com/svn/tags/markerclustererplus/2.0.1/src/markerclusterer_packed.js');
      drupal_add_js('http://google-maps-utility-library-v3.googlecode.com/svn/trunk/infobubble/src/infobubble-compiled.js');
      drupal_add_js(drupal_get_path('module', 'feature_branches') . '/js/feature_branches.js');

      $output = '';
      //$output .= '<p>'. check_plain($node->title) .'</p>';
      $street = field_get_items('node', $node, 'field_branch_address_str');
      $output .= '<div>' . check_plain($node->title) . check_plain($street[0]['value']) .' ';
//      $output .= check_plain($node->field_branch_address_nr[$node->language][0]['value']) .'<br/>';
      $zip = field_get_items('node', $node, 'field_branch_address_zip');
      $output .= check_plain($zip[0]['value']) .' ';
      $city = field_get_items('node', $node, 'field_branch_address_city');
      $output .= check_plain($city[0]['value']) .'</div>';
      $url = 'http://maps.google.be/maps';

      $node_fields = array('str', 'nr', 'zip', 'city');
      $address = array();

      foreach ((array) $node_fields as $node_field) {
        if (!empty($node->{'field_branch_address_' . $node_field}) && !empty($node->{'field_branch_address_' . $node_field}[$node->language][0]['value'])) {
          $address[] = $node->{'field_branch_address_' . $node_field}[$node->language][0]['value'];
        }
      }

      if (count($address)) {
        $address = join(' ', $address);
        $address = urlencode($address);
      }

      $output .= l(t('Calculate route'), $url, array('query' => array('daddr' => $address), 'attributes' => array('target' => '_blank')));

      drupal_add_js(array(
          'agent_gmap' => array(
              'title' => check_plain($node->title),
              'content' => $output,
              'lat' => $node->field_branch_google_lat[$node->language][0]['safe_value'],
              'lng' => $node->field_branch_google_lon[$node->language][0]['safe_value'],
          )
              ), 'setting'
      );

      return '<div id="agent_gmap"></div>';
    }
  }
}