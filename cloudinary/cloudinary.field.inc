<?php

require_once('lib/src/Cloudinary.php');

/**
 * Implements hook_field_formatter_info().
 */
function cloudinary_field_formatter_info() {
  $formatters = array(
    'cloudinary_image' => array(
      'label' => t('Cloudinary Image'),
      'field types' => array('image'),
      'settings' => array('image_style' => '', 'image_link' => '', 'image_rendered_as_src' => false),
    ),
  );

  return $formatters;
}
/**
 * 
 * Implements hook_file_formatter_info
 */

function hook_file_formatter_info() {
  $formatters['cloudinary_file_image'] = array(
    'label' => t('Cloudinary File Image'),
    'file types' => array('video'),
    'default settings' => array(
      'image_style' => '',
    ),
    'view callback' => 'cloudinary_file_formatter_image_view',
    'settings callback' => 'cloudinary_file_formatter_image_settings',
    'mime types' => array('image/cloudinary'),
  );

  return $formatters;
}

/**
 * Implements hook_file_formatter_FORMATTER_view().
 */
function cloudinary_file_formatter_image_view($file, $display, $langcode) {
	
  $scheme = file_uri_scheme($file->uri);
  if ($scheme == 'cloudinary') {
    $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri);
    $image_style = $display['settings']['image_style'];
    $valid_image_styles = image_style_options(FALSE);

    // @TODO: If autosubmit is removed and we allow view modes that insert
    // images in the WYSIWYG, add file->overrides handling.
    if (empty($image_style) || !isset($valid_image_styles[$image_style])) {
      $element = array(
        '#theme' => 'image',
        '#path' => $wrapper->getOriginalThumbnailPath(),
        '#alt' => isset($file->override['attributes']['alt']) ? $file->override['attributes']['alt'] : $file->filename,
      );
    }
    else {
      $element = array(
        '#theme' => 'image',
        '#path' => $wrapper->getOriginalThumbnailStylePath($image_style),
        '#alt' => isset($file->override['attributes']['alt']) ? $file->override['attributes']['alt'] : $file->filename,
      );
    }

    return $element;
  }
}

/**
 * Implements hook_file_formatter_FORMATTER_settings().
 */
function cloudinary_file_formatter_image_settings($form, &$form_state, $settings) {
  $element = array();
  $element['image_style'] = array(
    '#title' => t('Image style'),
    '#type' => 'select',
    '#options' => image_style_options(FALSE),
    '#default_value' => $settings['image_style'],
    '#empty_option' => t('None (original image)'),
  );
  return $element;
}


/**
 * Implements hook_field_formatter_settings_form().
 */
function cloudinary_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $image_styles = image_style_options(FALSE, PASS_THROUGH);
  $element['image_style'] = array(
    '#title' => t('Image style'),
    '#type' => 'select',
    '#default_value' => $settings['image_style'],
    '#empty_option' => t('None (original image)'),
    '#options' => $image_styles,
  );
  
  $element['image_rendered_as_src'] = array(
    '#title' => t('Image rendered as src'),
    '#type' => 'checkbox',
    '#default_value' => $settings['image_rendered_as_src'],
  );
  
  $link_types = array(
    'content' => t('Content'),
    'file' => t('File'),
  );

  $element['image_link'] = array(
    '#title' => t('Link image to'),
    '#type' => 'select',
    '#default_value' => $settings['image_link'],
    '#empty_option' => t('Nothing'),
    '#options' => $link_types,
  );

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function cloudinary_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $summary = array();

  $image_styles = image_style_options(FALSE, PASS_THROUGH);

  // Unset possible 'No defined styles' option.
  unset($image_styles['']);

  // Styles could be lost because of enabled/disabled modules that defines
  // their styles in code.
  if (isset($image_styles[$settings['image_style']])) {
    $summary[] = t('Image Style: @style and hosted on Cloudinary', array('@style' => $image_styles[$settings['image_style']]));
  }
  else {
    $summary[] = t('Original Image hosted on Cloudinary ');
  }
  
  if ($settings['image_rendered_as_src']) $summary[] = t('Image rendered as src url.');

  $link_types = array(
    'content' => t('Linked to content'),
    'file' => t('Linked to file'),
  );

  // Display this setting only if image is linked.
  if (isset($link_types[$settings['image_link']])) {
    $summary[] = $link_types[$settings['image_link']];
  }

  return implode('<br />', $summary);
}

/**
 * Implements hook_field_formatter_view().
 */
function cloudinary_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {

  $element = array();

  // Check if the formatter involves a link.
  if ($display['settings']['image_link'] == 'content') {
    $uri = entity_uri($entity_type, $entity);
  }
  elseif ($display['settings']['image_link'] == 'file') {
    $link_file = TRUE;
  }

  foreach ($items as $delta => $item) {
    if (isset($link_file)) {
      $uri = array(
        'path' => file_create_url($item['uri']),
        'options' => array(),
      );
    }
    
    $element[$delta] = array(
      '#theme' => 'image_formatter_cloudinary',
      '#item' => $item,
      '#image_style' => $display['settings']['image_style'],
      '#path' => isset($uri) ? $uri : '',
    	'#image_rendered_as_src' => $display['settings']['image_rendered_as_src'],
    );
  }

  return $element;
}

/**
 * Returns the URL for a cloudinary image given a style and filename.
 *
 * @param $style_name
 *   The name of the style to be used with this image.
 * @param $filename
 *   The filename of the image.
 *
 * @return
 *   The absolute URL where a style image can be downloaded.
 */
function cloudinary_style_url($style_name, $filename) {
  $variables = array();

  $style = image_style_load($style_name);

  $scheme = file_uri_scheme($filename);
  if ($scheme == 'cloudinary') {
    $wrapper = file_stream_wrapper_get_instance_by_uri($filename);
    $properties = $wrapper->get_parameters();

    $variables['cloudinary_id'] = $properties['id'];
    $variables['extension'] = $properties['ext'];

    $transformations = array();
    if (is_array($style) && array_key_exists('effects',$style)){
      foreach ($style['effects'] as $effect) {
        foreach ($effect['data'] as $key => $value) {
          if ($value == '') {
            unset($effect['data'][$key]);
          }
        }
        $transformations[] = $effect['data'];
      }
    }
    $variables['transformations'] = $transformations;
    return theme('image_src_cloudinary', $variables);
  }

  return image_style_url($style, $filename);
}

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

  if($extension == 'jpg')
  {
    if(!isset($options['transformation'][0])) $options['transformation'][0] = array();
    $options['transformation'][0]['quality'] = 'jpegmini';
    $options['sign_url'] = true;
  }

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

  if($extension == 'jpg')
  {
    if(!isset($options['transformation'][0])) $options['transformation'][0] = array();
    $options['transformation'][0]['quality'] = 'jpegmini';
    $options['sign_url'] = true;
  }

  return cloudinary_url($source, $options);
}