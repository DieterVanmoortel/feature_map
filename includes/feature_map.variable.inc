<?php
/**
 * @file
 * Definition of all custom variables
 * Variable module hook implementations
 */


/**
 * Implements hook_variable_group_info().
 */
function feature_map_variable_group_info() {
  $groups['map'] = array(
    'title' => t('Maps'),
    'description' => t('Map settings.'),
    'access' => 'administer map',
  );
  return $groups;
}

/**
 * Implements hook_variable_info().
 */
function feature_map_variable_info($options) {
  $variables['map_entity_type'] = array(
    'title' => t('Mapped content'),
    'type' => 'options',
    'group' => 'map',
    'options callback' => 'map_view_content_types',
    'default' => array(),
  );
//  $variables['map_max_results'] = array(
//    'title' => t('Maximum number of results to show: number'),
//    'type' => 'number',
//    'group' => 'map',
//    'default' => 10,
//  );
//  $variables['map_min_results'] = array(
//    'title' => t('Minimum number of results to show: number'),
//    'type' => 'number',
//    'group' => 'map',
//    'default' => 3,
//  );
//  $variables['map_min_results'] = array(
//    'title' => t('Maximum distance for minimum results (m)'),
//    'type' => 'number',
//    'group' => 'map',
//    'default' => 20000,
//  );
//  $variables['map_max_distance'] = array(
//    'title' => t('Maximum distance to look for results (m)'),
//    'type' => 'number',
//    'group' => 'map',
//    'default' => 40000,
//  );
  $variables['always_rebuild_coordinates'] = array(
    'title' => t('Always rebuild coordinates on update'),
    'type' => 'number',
    'group' => 'map',
    'default' => 0,
  );
  $variables['map_view_type'] = array(
    'title' => t('Map view type'),
    'type' => 'select',
    'group' => 'map',
    'options callback' => 'map_view_type_options',
    'default' => 'ROADMAP',
  );
  if(module_exists('ds')) {
    $variables['ds_buildmode_infowindow'] = array(
      'title' => t('DS Buildmode infowindow'),
      'type' => 'select',
      'group' => 'map',
      'options callback' => 'view_ds_types',
    ); 
  }
  
  return $variables;
}

