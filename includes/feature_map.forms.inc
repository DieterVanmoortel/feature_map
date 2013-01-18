<?php
/**
 * Callback for config/map/%entity_type
 * @param type $form
 * @param array $form_state
 * @param type $entity_type
 * @return type
 */
function map_entity_mapping_form($form, $form_state, $entity_type){
  $form = array();
  $form['#submit'][] = 'map_entity_mapping_form_submit';
  $options = array();
  $form_state['entity_type'] = $entity_type;
  
  $entity_info = field_info_instances();
  foreach((array)$entity_info as $type => $bundle) {
    if(array_key_exists($entity_type, $bundle)) {
      foreach((array)$bundle[$entity_type] as $key => $value) {
        $options[$key] = $value['label'];
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
        '#default_value' => variable_get($entity_type .'_mapping_country_check', '1'),
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

  if(isset($form_state['results'])){
    if(count($form_state['results']) != 0) {
      foreach((array)$form_state['results'] as $entity => $result){
        $grids = array();
        $title = $result['title'];
        unset($result['title']);
        unset($result['vid']);

        foreach((array)$result as $key => $obj){
          $grids[$key] = $obj->address;
        }
        $default_value = count($result) == 1 ? array_keys($grids) : ''; 
        $form[$entity_type .'_set_grid_' . $entity] = array(
          '#title' => t('Select address for ' . $title),
          '#type' => 'select',
          '#options' => $grids,
          '#default_value' => $default_value,
          );
        }
        $form['savetotable'] = array(
          '#type' => 'submit',
          '#value' => t('Save coordinates')
        );
        return $form;
    }
    elseif(count($form_state['results']) == 0){
      drupal_set_message(t('No content was found that has not yet been located.'));
    }
  }

  if(variable_get($entity_type .'_mapping', '') != '') {
    $form['locate'] = array(
      '#type' => 'submit',
      '#value' => t('Save configuration & Fetch coordinates')
    );    
  }
  
  return system_settings_form($form);
}

/**
 *  Submit handler for config/map/%entity_type
 * @param type $form
 * @param type $form_state
 */
function map_entity_mapping_form_submit($form, &$form_state) {
  // if fetch coordinates is clicked
  if($form_state['clicked_button']['#id'] == 'edit-locate'){
    $form_state['results'] = feature_map_check_coordinates($form_state['entity_type'], TRUE);
    $form_state['rebuild'] = TRUE;
  }
  // if save coordinates is clicked
  elseif($form_state['clicked_button']['#id'] == 'edit-savetotable'){
    foreach((array)$form_state['results'] as $key => $result) {
      
      $selection = $form_state['values'][$form_state['entity_type'] . '_set_grid_' .$key];
      db_merge('map_coordinates')
        ->key(array('entity_id' => $result['vid']))
        ->fields(array(
            'lon' => $result[$selection]->Point->coordinates[0],
            'lat' => $result[$selection]->Point->coordinates[1],
        ))
        ->execute();
    }
  }
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
    $coords = retrieve_map_coordinates($address, $entity, $display);
    $coords ? $results = array_merge($results, $coords): '';
  }
  return $display ? $results : $count . ' nodes were enriched with lattitude & longitude.';
}

function feature_map_node_settings($form_id, $form_state, $nid){
  $lat = '';
  $lon = '';

  $coord = db_query('SELECT lat, lon FROM {map_coordinates} WHERE entity_id = ' . $nid)->fetchAll();

  if ($coord && isset($coord[0])) {
    $lat = $coord[0]->lat;
    $lon = $coord[0]->lon;
  }
  else{
    drupal_set_message(t('No coordinates mapped.'));
  }
  
  $form['location'] = array(
    '#prefix' => '<div id="gmap-preview">',
    '#suffix' => '</div>',
  );

  $form['location']['lat'] = array(
    '#type' => 'textfield',
    '#title' => 'Latitude',
    '#default_value' => $lat,
    '#prefix' => '<div class="coordinates">',
  );
  $form['location']['lon'] = array(
    '#type' => 'textfield',
    '#title' => 'Longitude',
    '#default_value' => $lon,
    '#suffix' => '</div>',
  );
  $form['location']['map'] = array(
    '#markup' => theme('entity_preview', array($lat, $lon)),
  );
  
  $entity = entity_load('node', array($nid));
  if(is_array($entity)){
    $entity = $entity[$nid];
    $mapped_fields = variable_get_value($entity->type .'_mapping');
    $addresscomponents = array();
    foreach((array)$mapped_fields as $field) {
      $value = field_get_items('node', $entity, $field);
      $addresscomponents[] = strtolower($value[0]['value']);
    }
    $address = implode(',', $addresscomponents);
    //  url needs to match with the one in map_fetch_grid() (Helpers)
    $url = "http://maps.googleapis.com/maps/api/geocode/json?address=";
    $url .= $address;
    $url .= "&sensor=false";
    $form['location']['gmap-link'] = array(
      '#markup' => l('Check on Google Maps', $url, array('attributes' => array('target' => 'blanco'))),
    );
  }
  return system_settings_form($form);

}

function feature_map_node_settings_validate($form, $form_state) {
  $lat = $form_state['values']['lat'];
  $lon = $form_state['values']['lon'];
  if(!is_numeric($lat) || !is_numeric($lon) || $lat > 90 || $lat < 0 || $lon < -180 || $lon > 180){
    form_set_error('', t('Invalid latitude or longitude value'));
  }
}

function feature_map_node_settings_submit($form, $form_state) {
  if(is_numeric(arg(1))){
    db_merge('map_coordinates')
    ->key(array('entity_id' => arg(1)))
    ->fields(array(
      'lat' => $form_state['values']['lat'],
      'lon' => $form_state['values']['lon'],
    ))
    ->execute();
  }
}
