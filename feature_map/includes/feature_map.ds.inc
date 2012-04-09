<?php

/**
 * Display suite: Calculate route
 */
function _render_calculate_route_content($field) {
  $node = $field['entity'];

  $url = 'http://maps.google.be/maps';


  $node_fields = array('str', 'nr', 'zip', 'city');
  $address = array();
  // TODO : use mapping !! 
  
  foreach ((array) $node_fields as $node_field) {
    if(!empty($node->{'field_branch_address_' . $node_field}) && !empty($node->{'field_branch_address_' . $node_field}[$node->language][0]['value'])) {
      $address[] = $node->{'field_branch_address_' . $node_field}[$node->language][0]['value'];
    }
  }

  if (count($address)) {
    $address = join(' ', $address);
    $address = urlencode($address);
  }

  return l(t('Calculate route'), $url, array('query' => array('saddr' => $address), 'attributes' => array('target' => '_blank')));
}
