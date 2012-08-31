<?php
chdir($_SERVER['DOCUMENT_ROOT']);
define('DRUPAL_ROOT', getcwd());

require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_DATABASE);