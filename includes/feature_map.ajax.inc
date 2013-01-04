<?php

/**
 * Branch locator JSON callback
 * ajax/branch-locator
 */
function feature_map_locator_json() {
  $data = isset($_REQUEST['markers']) ? check_plain($_REQUEST['markers']) : 'initial';

  $markers = load_all_markers();

  if($data <> 'initial') {
    // do some extra filtering
  }

  

//  return 'dev output'; //dev
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
        'title' => $full_entity->title,
        'lat' => $entity->lat,
        'lng' => $entity->lon,
        'id' => $entity->entity_id
      );
      // add filters
      $filters = variable_get($full_entity->type .'_mapfilters', FALSE);
      if ($filters) {
        foreach((array)$filters as $filter) {
          $fieldvalue = field_get_items($entity->entity_type, $full_entity, $filter);
          $marker[$filter] = $fieldvalue[0]['value'];
        }
      }
      $markers[] = array(
        'lat' => $marker['lat'],
        'lng' => $marker['lng'],
        'teaser' => $marker['title'],
        'full' => '<li>' . $marker['title'] . '</li>',
      );
    }
  }
  $stored_markers = $markers;
  return $markers;
}
