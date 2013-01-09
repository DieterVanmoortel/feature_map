<?php

/**
 * Branch locator JSON callback
 * ajax/branch-locator
 */
function feature_map_locator_json() {
  $data = isset($_REQUEST['markers']) ? check_plain($_REQUEST['markers']) : 'initial';

  $markers = load_all_markers();

  if($data <> 'initial') {
    // do some default filtering
  }

  // alter filters by a hook?
//  apply_markers_alter($markers);
  
  foreach($markers as $key => $marker){
    if (isset($marker['field_event_date'])) {
      $markers[$key]['month'] =  format_date($marker['field_event_date'], 'custom', 'n');
    }
  }
//  return '';
  return drupal_json_output($markers);
}


function load_all_markers(){ 
  $stored_markers = &drupal_static(__FUNCTION__);
//  if (isset($stored_markers)) {return $stored_markers; }
  
  $markers = array();
  $entities = db_query('SELECT * FROM {map_coordinates}')->fetchAll();
  foreach((array)$entities as $entity) {
    $full_entity = entity_load($entity->entity_type, array($entity->entity_id));
    if ($full_entity) {
      $full_entity = $full_entity[$entity->entity_id];
      $marker = array(
        'teaser' => $full_entity->title,
        'full' => $full_entity->title,
        'lat' => $entity->lat,
        'lng' => $entity->lon,
        'id' => $entity->entity_id
      );
      // add filters
      $filters = variable_get($full_entity->type .'_mapfilters', FALSE);
      if ($filters) {
        foreach((array)$filters as $filter) {
          $fieldvalue = field_get_items($entity->entity_type, $full_entity, $filter);
          if (isset($fieldvalue[0]['value'])) {
            $marker[$filter] = $fieldvalue[0]['value'];
          }
        }
      }
      $markers[] = $marker;
    }
  }
  $stored_markers = $markers;
  return $markers;
}


  function apply_markers_alter(&$markers) {
    $mods = module_implements('markers_alter');
    $alters = array();
    if ($cache = cache_get('markers_alter')) {
      $alters = $cache->data;
    }
    else {
      foreach((array) $mods as $mod){
        $callback = $mod . '_markers_alter';
        $alterations = $callback();
        foreach((array)$alterations as $alteration) {
          if(isset($alteration['file'])){
            $alters['files'][$mod][] = $alteration['file'];
          }
        }
      }
      cache_set('markers_alter', $checks, 'cache');
    }
    // make sure all files are loaded
    if(isset($alters['files'])) {
      foreach($alters['files'] as $mod => $files){
        foreach((array)$files as $file) {
        require_once(drupal_get_path('module', $mod) . '/' . $file);
        }
      }
    }
    foreach((array)$alters as $alter) {
      $alter($markers);
    }
  }