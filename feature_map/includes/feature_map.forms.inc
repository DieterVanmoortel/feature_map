<?php

/*
 * Implementing hook_form_alter
 */
function feature_map_form_alter(&$form, &$form_state, $form_id){
  switch($form_id) {
    case 'variable_module_form':
      $form['#submit'][] = 'feature_map_rebuild_menu';
      break;
  }
}

function feature_map_rebuild_menu(){
  if(menu_rebuild()){drupal_set_message(t('Menu rebuilt'));}
}

/**
 * Callback for config/map/%entity_type
 * @param type $form
 * @param array $form_state
 * @param type $entity_type
 * @return type
 */
function map_entity_mapping_form($form, $form_state, $entity_type){
  $form = array();
  $options = array();
  $form_state['entity_type'] = $entity_type;
  $entity_info = field_info_instances();
  foreach((array)$entity_info as $type => $bundle) {
    if(array_key_exists($entity_type, $bundle)) {
      foreach((array)$bundle as $key => $value) {
        foreach((array)$value as $label => $field){
          $options[$label] = $field['label'];
        }             
      }
    }
  }
  unset($options['body']);
  if (count($options)){

    $form[$entity_type .'_mapping'] = array(
        '#title' => t('Fields that hold location data'),
        '#type' => 'select',
        '#options' => $options,
        '#multiple' => TRUE,
        '#size' => 5,
        '#default_value' => variable_get($entity_type .'_mapping', ''),
        '#description' => t('Choose which fields you want to use to send to Google maps to fetch location data. Use fields that hold address data, such as city, country or zipcode.')
    );
    $form[$entity_type .'_mapping_country_check'] = array(
        '#title' => t('All entities are situated in the same country.'),
        '#type' => 'checkbox',
        '#default_value' => variable_get($entity_type .'_mapping_country_check', ''),
        '#description' => t('Check this box if you have no fields that return country information.'),
    );
    $countries = map_selected_countries();
    $form[$entity_type .'_mapping_country'] = array(
        '#title' => t('Set default country.'),
        '#type' => 'radios',
        '#options' => $countries,
        '#default_value' => variable_get($entity_type .'_mapping_country', 'BE'),
        '#description' => t('Check this box if you have no fields that return country information.'),
        '#states' => array(
          'visible' => array(
          '#edit-city-mapping-country-check' => array('checked' => TRUE),
    ),
        )
    );
    $form[$entity_type .'_mapfilters'] = array(
        '#title' => t('Fields that will be used as filters'),
        '#type' => 'select',
        '#options' => $options,
        '#multiple' => TRUE,
        '#size' => 5,
        '#default_value' => variable_get($entity_type .'_mapfilters', ''),
        '#description' => t('Choose which fields you want to use to results.')
    );
   
  } else {
      $form['no_values'] = array(
          '#markup' => t('No fields were found to map this entity type.')
      );
  }
  if(variable_get($entity_type .'_mapping', '') != ''){
    $form['locate'] = array(
      '#type' => 'submit',
      '#value' => t('Fetch coordinates')
    );
  }
  if(isset($form_state['results'])) {
    foreach((array)$form_state['results'] as $entity => $result){
      // below is useless when there's only one result.
      if(count($result) == 1) { continue; }
      $grids = array();
      foreach((array)$result as $key => $obj){
        dpm($obj, 'result returned by google maps for ' .$entity);
        $grids[$key] = $obj->AddressDetails->Country->CountryName;
      }
      $form[$entity_type .'_set_grid_' . $entity] = array(
        '#title' => t('Select country for ' . $entity),
        '#type' => 'radios',
        '#options' => $grids,
        '#default_value' => variable_get($entity_type .'_set_grid_' . $entity, 'BE'),
        );
    }
  }
  $form['#submit'][] = 'map_entity_mapping_form_submit';
  return system_settings_form($form);
}

/**
 *  Submit handler for config/map/%entity_type
 * @param type $form
 * @param type $form_state
 */
function map_entity_mapping_form_submit($form, &$form_state) {
  if($form_state['clicked_button']['#id'] == 'edit-locate'){
    $form_state['results'] = feature_map_check_coordinates($form_state['entity_type'], TRUE);
    $form_state['rebuild'] = TRUE;
  }
//  elseif(isset($form_state['found_grids'])){unset($form_state['found_grids']);}
}



  /**
 * Cron callback
 * menu : admin/settings/check-coordinates
 * @return type
 */
function feature_map_check_coordinates($entity_type = FALSE, $display = FALSE) {
  $results = array();
  // need to fix this so that we can use it for ALL entities, not just nodes.. 
  $query = db_select('node', 'n');
  $entity_type ?  $query->condition('type', $entity_type) : '';
  $query->leftjoin('map_coordinates', 'grid', 'n.nid = grid.entity_id');
  $query->condition('grid.lat', NULL);
  $query->condition('grid.lon', NULL);
//  $query->condition('field_branch_address_zip_value', NULL, 'IS NOT');
  $query->range(0,100);
  $query->condition('n.status', 1);
  $query->fields('n', array('nid'));
  $result = $query->execute()->fetchCol();

  // switch this to entities TODO
  $entities = node_load_multiple($result);
  $count = count($entities);
  foreach((array) $entities as $entity) {
    $address = $data = array();
    $mapped_fields = variable_get_value($entity_type .'_mapping');
      foreach((array)$mapped_fields as $key => $field) {
        $data = field_get_items('node', $entity, $field);
        $address[] = $data[0]['value'];
      }
    $results = array_merge($results, retrieve_map_coordinates($address, $entity, $display));
  }

  return $display ? $results : $count . ' nodes were enriched with lattitude & longitude.';
}