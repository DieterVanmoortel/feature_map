<?php

/**
 * Branch locator JSON callback
 * ajax/branch-locator
 */
function feature_map_locator_json() {
  // Values
  $vars = array(
      'language' => isset($_REQUEST['language']) ? check_plain($_REQUEST['language']) : 'fr',
      'lat_1' => isset($_REQUEST['nelat']) ? check_plain($_REQUEST['nelat']) : 0,
      'lat_2' => isset($_REQUEST['swlat']) ? check_plain($_REQUEST['swlat']) : 0,
      'lon_1' => isset($_REQUEST['nelng']) ? check_plain($_REQUEST['nelng']) : 0,
      'lon_2' => isset($_REQUEST['swlng']) ? check_plain($_REQUEST['swlng']) : 0,
      'postal_code' => isset($_REQUEST['postcode']) ? check_plain($_REQUEST['postcode']) : FALSE,
      'initial' => isset($_REQUEST['initial']) ? $_REQUEST['initial'] : FALSE,
      'totalbranches' => isset($_REQUEST['totalbranches']) ? $_REQUEST['totalbranches'] : FALSE,
      'filters' => array(),
  );

  // Filters
  $available_filters = array(
      'saving_products',
      'banking_products',
      'atm',
//      'selfrecord',
      'credits_all',
      'credits_energy',
      'credits_home',
      'credits_cash',
      'credits_car',
      'credits_other',
  );

  foreach ($available_filters as $filter) {
    if (isset($_REQUEST[$filter])) {
      $vars['filters'][$filter] = TRUE;
    }
  }
  // if only credits_all is set, activate the credits_energy filter.
  if(count($vars['filters'] == 1) && isset($vars['filters']['credits_all'])){
    $vars['filters']['credits_energy'] = TRUE;
  }
  $response = feature_map_search_sort($vars);
  return drupal_json_output($response);
}



/**
 * Performs a search with the given variables.
 */
function feature_map_search_sort($vars) {
  if( $vars['postal_code'] && $vars['postal_code'] != false ) {
  // Search for results in the given zipcode
    $branch_nids = features_branch_locator_search_postal($vars);
    $branchlist = node_load_multiple($branch_nids);
  }
  else{
  // Search for results in the given lat & lon ranges.
    $branch_nids = feature_branch_locator_search_proximity($vars);
    foreach((array)$branch_nids as $distance => $nid) {
      $branchlist[$distance] = node_load($nid);
    }
  }
  
  
 /*
  * FILTERING & SORTING ALGORYTHMS
  */
  $ranking = array();
  // when filters are selected 
  if(count($vars['filters']) > 0) {
    
    $sortclass = "";
    
    foreach ($branchlist as $nid => $node) {
      
      // If atm is selected : don't show branches with no atms.
      $atm = field_get_items('node', $node, 'field_branch_atm');
      $atm = $atm[0]['value'];
      if (isset($vars['filters']['atm']) == TRUE && $atm <> 1 ){
        continue;
      }

     // if banking is selected : show branches with banking 1 and order by ranking_banking
      if(isset($vars['filters']['banking_products'])|| isset($vars['filters']['saving_products']) ){
        $trigger = field_get_items('node', $node, 'field_branch_banking_products');
        $trigger = $trigger[0]['value'];
        $rankingvalue = field_get_items('node', $node, 'field_branch_ranking_banking');
        $rankingvalue = $rankingvalue[0]['value'];
        if($trigger == 1){
          if(!empty($rankingvalue)){
            $ranking[$nid] = $rankingvalue;
          }
          else {$ranking[$nid] = 1000;}
          continue;
        }
      }
     // credits_energy, credits_home with credits_other
      elseif(isset($vars['filters']['credits_energy'] )|| (isset($vars['filters']['credits_home']) && isset($vars['filters']['credits_other']))) {
        $trigger = field_get_items('node', $node, 'field_branch_credits_all');
        $trigger = $trigger[0]['value'];
        $rankingvalue = field_get_items('node', $node, 'field_branch_ranking_credits');
        $rankingvalue = $rankingvalue[0]['value'];
        if($trigger == 1){
          if(!empty($rankingvalue)){
            $ranking[$nid] = $rankingvalue;
          }
          else {$ranking[$nid] = 1000;}
          continue;
        }
      }
     // credits_home without credits_other
      elseif(isset($vars['filters']['credits_home']) && !isset($vars['filters']['others']) ){
        $trigger = field_get_items('node', $node, 'field_branch_credits_home');
        $trigger = $trigger[0]['value'];
        $rankingvalue = field_get_items('node', $node, 'field_branch_ranking_home');
        $rankingvalue = $rankingvalue[0]['value'];
        if($trigger == 1){
          if(!empty($rankingvalue)){
            $ranking[$nid] = $rankingvalue;
          }
          else {$ranking[$nid] = 1000;}
          continue;
        }
      }
     // only credits_other
      elseif(isset($vars['filters']['credits_other'])) {
        $trigger = field_get_items('node', $node, 'field_branch_credits_other');
        $trigger = $trigger[0]['value'];
        $rankingvalue = field_get_items('node', $node, 'field_branch_ranking_loans');
        $rankingvalue = $rankingvalue[0]['value'];
        if($trigger == 1){
          if(!empty($rankingvalue)){
            $ranking[$nid] = $rankingvalue;
          }
          else {$ranking[$nid] = 1000;}
          continue;
        }
      }
    }
    asort($ranking);
    foreach ((array)$ranking as $nid => $item) {
      $agentset[$nid] = $branchlist[$nid];
    }
  }   
  else{
    $agentset = $branchlist;
  }
  if ($vars['initial']){
    $sortclass = "sort";
    $agentset = array_slice($agentset, 0, (BRANCHES_MAXIMUM_RESULTSET_COUNT - $vars['totalbranches']));
  }
  
  
  // Build the agents array to return.
  $nodes = array();

  // Loop trough all results
  foreach ((array) $agentset as $node) {
    $agents[] = array(
        'id' => $node->nid,
        'name' => check_plain($node->title),
        'lat' => $node->field_branch_google_lat[$node->language][0]['value'],
        'lng' => $node->field_branch_google_lon[$node->language][0]['value'],
        'marker_icon' => "/sites/all/themes/benny/images/map/" . feature_branches_marker_icon($node),
        'tab_info_html' =>  theme('branch_info_tab', array('node' => $node, 'language' => $vars['language'])),
        'tab_hours_html' => theme('branch_hours_tab', array('node' => $node, 'language' => $vars['language'])),
        'tab_services_html' => theme('branch_services_tab', array('node' => $node, 'language' => $vars['language'])),
        'list_item_html' => theme('branch_list_item', array('node' => $node, 'language' => $vars['language'], 'class' => $sortclass)),
        'atm' => $node->field_branch_atm[$node->language][0]['value'],
    );
  }
  return array('nodes' => $nodes);
}