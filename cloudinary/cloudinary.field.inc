<?php

require_once('lib/src/Cloudinary.php');
require_once('cloudinary.theme.inc');

/**
 * Implements hook_field_formatter_info().
 */
function cloudinary_field_formatter_info() {
  $formatters = array(
    'cloudinary_image' => array(
      'label' => t('Cloudinary Image'),
      'field types' => array('image'),
      'settings' => array('horizontal_image_style' => '', 'vertical_image_style' => '', 'image_link' => '', 'image_rendered_as_src' => false),
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
  $element['horizontal_image_style'] = array(
    '#title' => t('Horizontal Image style'),
    '#type' => 'select',
    '#default_value' => $settings['horizontal_image_style'],
    '#empty_option' => t('None (original image)'),
    '#options' => $image_styles,
  );
  
  $element['vertical_image_style'] = array(
    '#title' => t('Vertical Image style'),
    '#type' => 'select',
    '#default_value' => $settings['vertical_image_style'],
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
  
  if (isset($image_styles[$settings['horizontal_image_style']]) || isset($image_styles[$settings['vertical_image_style']])) {
    $summary[] = t('Image Styles (Horizontal/Vertical): @horizontal_image_style, @vertical_image_style and hosted on Cloudinary',
        array(
            '@horizontal_image_style' => isset($image_styles[$settings['horizontal_image_style']]) ? $image_styles[$settings['horizontal_image_style']] : 'original (unstyled)',
            '@vertical_image_style' => isset($image_styles[$settings['vertical_image_style']]) ? $image_styles[$settings['vertical_image_style']] : 'original (unstyled)',
        ));
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
    
    if (!empty($item['metadata']['width']) && !empty($item['metadata']['height'])) $style_selected = $item['metadata']['width'] >= $item['metadata']['height'] ? 'horizontal_image_style' : 'vertical_image_style';
    else $style_selected = 'horizontal_image_style';

    $image_style = $display['settings'][$style_selected];

    $element[$delta] = array(
      '#theme' => 'image_formatter_cloudinary',
      '#item' => $item,
      '#image_style' => $image_style,
      '#path' => isset($uri) ? $uri : '',
   '#image_rendered_as_src' => $display['settings']['image_rendered_as_src'],
    );
  }

  return $element;
}

