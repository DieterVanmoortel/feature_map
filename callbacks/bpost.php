<?php
$q = isset($_GET["q"]) ? $_GET["q"] : "";
$lang = (isset($_GET["lang"]) && $_GET["lang"] == "nl" ? "nl" : "fr");
$otherlang = ($lang = "fr" ? "nl" : "fr");
ob_start();
include 'bpost.json';
$zipcodes = ob_get_contents();
ob_end_clean();

$result = "";
if (empty($q)) {
  $result = $zipcodes;
} 
else if (is_numeric($q)) {
//    $pattern='/{[\s]*[^}]*"zipcode":'.$q.'[0-9]{0,4}"[^}]*}/msiU';
  $pattern = '/{[\s]*[^}]*"zipcode":"' . $q . '[0-9]{0,4}"[^}]*}/msiU';
  if ($debug)
    print $pattern;
  $matchcount = preg_match_all($pattern, $zipcodes, $matches);
  //if($debug) echo "<pre>".print_r($matches,true)."</pre>";
  $result = ($matchcount == 0) ? "[]" : "[" . implode(",", $matches[0]) . "]";
} 
else {
  $pattern = '/{[\s]*[^}]*"name_' . $lang . '":"' . $q . '[^"]*' . '"[^}]*}/msiU';
  if ($debug)
    print $pattern;
  $matchcount = preg_match_all($pattern, $zipcodes, $matches);
  $otherpattern = '/{[\s]*[^}]*"name_' . $otherlang . '":"' . $q . '[^"]*' . '"[^}]*}/msiU';
  $result = ($matchcount == 0) ? "[]" : "[" . implode(",", $matches[0]) . "]";
}
print $result;