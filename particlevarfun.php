
<?php
// Based on Simple Spark PHP Proxy by wgbartley

$device = $_REQUEST["device"];
$method = $_REQUEST["method"];
if(!$device||!$method) die("device or method param missing\n");

// add access token
$_REQUEST["access_token"] = "<<your_access_token_here>>" ;

$args =$_REQUEST["args"];
if(!$args) {
// create params
$params = http_build_query($_REQUEST);
// make request
$url = "https://api.spark.io/v1/devices/" . $device . "/" . $method . "?" . $params;
$response = file_get_contents($url);
echo $response;}

else 

{

// Create a stream
$opts = array(   
  'http'=>array( 
    'method'=>"POST",
      'header'=>"Authorization: Bearer ".$_REQUEST['access_token']."\r\n",
     'content'=>"args=".$args
      )
);

$context = stream_context_create($opts);

// make request
$url = "https://api.spark.io/v1/devices/" . $device . "/" . $method;
$response = file_get_contents($url, false, $context);
echo $response;
}


// These are for debugging - uncomment to use
//echo " context: ", $context;
//echo $token;
//echo " params: ", $params;
//print_r($context);
//print_r($opts);
//print($args);
?>
