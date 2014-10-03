<?php

/**
 * Returns HTML for an image field formatter.
 *
 * @param $variables
 *   An associative array containing:
 *   - item: Associative array of image data, which may include "uri", "alt",
 *     "width", "height", "title" and "attributes".
 *   - image_style: An optional image style.
 *   - path: An array containing the link 'path' and link 'options'.
 *
 * @ingroup themeable
 */
function theme_image_formatter_cloudinary($variables) {
	
  $item = $variables['item'];
  
  $scheme = file_uri_scheme($item['uri']);
  if ($scheme != 'cloudinary') return theme_image_formatter($variables);
  
  // Retrieve parameters from cloudinary:// uri
	$mcsw = new MediaCloudinaryStreamWrapper();
	$mcsw->setUri($item['uri']);
	$mcsw_parameters = $mcsw->get_parameters();
  $image_renderable_array = array(
    'cloudinary_id' => $mcsw_parameters['id'],
    'extension' => $mcsw_parameters['ext'],
  );
  
  if (array_key_exists('alt', $item)) {
    $image_renderable_array['alt'] = $item['alt'];
  }

  if (isset($item['attributes'])) {
    $image_renderable_array['attributes'] = $item['attributes'];
  }

  if (isset($item['width']) && isset($item['height'])) {
    $image_renderable_array['width'] = $item['width'];
    $image_renderable_array['height'] = $item['height'];
  }

  // Do not output an empty 'title' attribute.
  if (isset($item['title']) && drupal_strlen($item['title']) > 0) {
    $image_renderable_array['title'] = $item['title'];
  }
	
  if (empty($variables['image_rendered_as_src'])) {
	  if ($variables['image_style']) {
	  	$image_renderable_array['style_name'] = $variables['image_style'];
	  	$output = theme('image_style_cloudinary', $image_renderable_array);
	  }
	  else $output = theme('image_cloudinary', $image_renderable_array);
  }
  else {
	  if ($variables['image_style']) {
	  	$image_renderable_array['style_name'] = $variables['image_style'];
	  	$output = theme('image_style_src_cloudinary', $image_renderable_array);
	  }
	  else $output = theme('image_src_cloudinary', $image_renderable_array);
  }
	  
  // The link path and link options are both optional, but for the options to be
  // processed, the link path must at least be an empty string.
  if (isset($variables['path']['path'])) {
    $path = $variables['path']['path'];
    $options = isset($variables['path']['options']) ? $variables['path']['options'] : array();

    // When displaying an image inside a link, the html option must be TRUE.
    $options['html'] = TRUE;
    $output = l($output, $path, $options);
  }

  return $output;
}

/**
 * Theme function for image style.
 *
 * @param $variables
 * @return string
 */
function theme_image_style_cloudinary($variables) {

  $style_name = $variables['style_name'];
  $style = image_style_load($style_name);

  $transformations = array();

  foreach ($style['effects'] as $effect) {
    foreach ($effect['data'] as $key => $value) {
      if ($value == '') {
        unset($effect['data'][$key]);
      }
    }
    $transformations[] = $effect['data'];
  }
  $variables['transformations'] = $transformations;
  return theme('image_cloudinary', $variables);
}

/**
 * Theme function for image style.
 *
 * @param $variables
 * @return string
 */
function theme_image_style_src_cloudinary($variables) {

  $style_name = $variables['style_name'];
  $style = image_style_load($style_name);

  $transformations = array();

  foreach ($style['effects'] as $effect) {
    foreach ($effect['data'] as $key => $value) {
      if ($value == '') {
        unset($effect['data'][$key]);
      }
    }
    $transformations[] = $effect['data'];
  }
  $variables['transformations'] = $transformations;
  return theme('image_src_cloudinary', $variables);
}

/**
 * Theme image output using Cloudinary settings.
 * Expects an image renderable array
 * @param $variables
 * @return string
 */
function theme_image_cloudinary($variables) {

  // Cloudinary
  _cloudinary_initialize();

  // Create options
  $options = array();

  if (isset($variables['transformations']) && is_array($variables['transformations']) && count($variables['transformations']) > 0) {
    $options["transformation"] = $variables['transformations'];
  }

  // Assign secure if we are delivering via https or Drupal is running from command-line
  if ( (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] != 'off') || drupal_is_cli() ) {
    $options['secure'] = TRUE;
  }

  // Create output
  $cloudinary_id = $variables['cloudinary_id'];
  $extension = isset($variables['extension']) ? $variables['extension'] : 'png';
  $source = $cloudinary_id . '.' . $extension;

  /*if($extension == 'jpg')
  {
    if(!isset($options['transformation'][0])) $options['transformations'][0] = array();
    $options['transformation'][0]['quality'] = 'jpegmini';
    $options['sign_url'] = true;
  }*/

  $variables['path'] = cloudinary_url($source, $options);
    
  // Cloudinary defines dynamic image sizes that might not be known at the time of image generation
  // Therefore unset the image width / height
  if (isset($variables["width"])) unset($variables["width"]);
  if (isset($variables["height"])) unset($variables["height"]);
  
  return theme('image', $variables);
}

/**
 * Theme image output using Cloudinary settings.
 * Expects an image renderable array
 * @param $variables
 * @return string
 */
function theme_image_src_cloudinary($variables) {

  // Cloudinary
  _cloudinary_initialize();

  // Create options
  $options = array();

  if (isset($variables['transformations']) && is_array($variables['transformations']) && count($variables['transformations']) > 0) {
    $options["transformation"] = $variables['transformations'];
  }

  // Assign secure if we are delivering via https or Drupal is running from command-line
  if ( (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] != 'off') || drupal_is_cli() ) {
    $options['secure'] = TRUE;
  }

  // Create output
	$cloudinary_id = $variables['cloudinary_id'];
	$extension = isset($variables['extension']) ? $variables['extension'] : 'png';
  $source = $cloudinary_id . '.' . $extension;

  /*if($extension == 'jpg')
  {
    if(!isset($options['transformation'][0])) $options['transformation'][0] = array();
    $options['transformation'][0]['quality'] = 'jpegmini';
    $options['sign_url'] = true;
  }*/

  return cloudinary_url($source, $options);
}