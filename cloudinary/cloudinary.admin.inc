<?php

/**
 * @file
 * Module administration.
 */

/**
 * Wrapper function to return teh settings page.
 * @return mixed
 */
function cloudinary_settings_form() {
  $form = array();

  $form['cloudinary_api'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cloudinary API Settings'),
    '#weight' => 5,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t("The information for this page can be found on your Cloudinary dashboard, <a href='https://cloudinary.com/console'>here</a>"),
  );

  $form['cloudinary_api']['cloudinary_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('API Key'),
    '#default_value' => variable_get('cloudinary_api_key', ''),
    '#description' => t("API Key - Found on Cloudinary dashboard"),
  );
  $form['cloudinary_api']['cloudinary_api_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('API Secret Key'),
    '#default_value' => variable_get('cloudinary_api_secret', ''),
    '#description' => t("API Secret - Found on Cloudinary dashboard"),
  );
  $form['cloudinary_api']['cloudinary_api_cloud_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Cloud Name'),
    '#default_value' => variable_get('cloudinary_api_cloud_name', ''),
    '#description' => t("Cloud Name - Found on Cloudinary dashboard"),
  );

  return system_settings_form($form);
}

function cloudinary_site_prefix_form($form, &$form_state) {

  $form['cloudinary_site_prefix_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cloudinary Site Prefix'),
    '#weight' => 5,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t("Each site using the same Cloudinary CDN must provide a prefix to use as a top level folder, so that sites don't overwrite each others' images."),
  );

  $prefix = variable_get('cloudinary_site_prefix', uniqid());

  $form['cloudinary_site_prefix_fieldset']['cloudinary_site_prefix'] = array(
    '#type' => 'textfield',
    '#title' => t('Cloudinary Site Prefix'),
    '#default_value' => $prefix,
    '#description' => t("Field that designates the top level Folder to upload the image to, defined per site."),
  );

  $form['cloudinary_site_prefix_fieldset']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update site prefix'),
  );

  $form['#submit'][] = 'cloudinary_site_prefix_form_submit';

  return $form;
}

function cloudinary_site_prefix_form_submit($form, &$form_state) {

  variable_set('cloudinary_site_prefix', $form_state['values']['cloudinary_site_prefix']);

  drupal_set_message(t('The folder configuration has been saved.'));
}

function cloudinary_context_mapping_settings_form($form, &$form_state) {

  // Output all current mappings and give option to clear
  // Clearing is permanent
  $form['cloudinary_context_mapping_settings_form_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cloudinary Context Field Mapping Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t("A collection of field mappings that will be used to extract the values of fields during upload and inserted into the image context in Cloudinary"),
  );

  // Output all current mappings and give option to clear
  // Clearing is permanent
  $form['cloudinary_context_mapping_settings_form_fieldset']['cloudinary_display_field_mapping_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('View Current Cloudinary Context Field Mappings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t("A collection of field mappings that will be used to extract the values of fields during upload and inserted into the image context in Cloudinary"),
  );

  $context_mapping = variable_get('cloudinary_field_mappings', array(
    "field_file_image_alt_text" => "alt",
    "field_file_image_title_text" => "caption"
  ));
  $context_mapping_row_data = array();
  foreach ($context_mapping as $field_id => $cloudinary_context_id) {
    $context_mapping_row_data[] = array($field_id, $cloudinary_context_id);
  }

  $mapping_table = array(
    'header' => array('Field Machine Name', 'Context Key'),
    'rows' => $context_mapping_row_data,
    'attributes' => array()
  );

  $form['cloudinary_context_mapping_settings_form_fieldset']['cloudinary_display_field_mapping_fieldset']['field_mappings'] = array(
    '#markup' => theme('table', $mapping_table)
  );

  $form['cloudinary_context_mapping_settings_form_fieldset']['cloudinary_display_field_mapping_fieldset']['clear'] = array(
    '#type' => 'submit',
    '#value' => t('Clear all mappings'),
  );

  $form['cloudinary_context_mapping_settings_form_fieldset']['cloudinary_add_to_field_mapping_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add field mapping to Cloudinary Context Field Mapping Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t("A collection of field mappings that will be used to extract the values of fields during upload and inserted into the image context in Cloudinary"),
  );

  $file_entity_image_fields = _cloudinary_retrieve_file_entity_image_fields();
  $file_entity_image_fields = drupal_map_assoc($file_entity_image_fields);

  $form['cloudinary_context_mapping_settings_form_fieldset']['cloudinary_add_to_field_mapping_fieldset']['file_entity_image_field'] = array(
    '#type' => 'select',
    '#title' => t('Field Machine Name'),
    '#options' => $file_entity_image_fields,
    '#description' => t("Field from which to extract data"),
  );

  $form['cloudinary_context_mapping_settings_form_fieldset']['cloudinary_add_to_field_mapping_fieldset']['cloudinary_context_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Cloudinary Context Key'),
    '#description' => t("Key to use when transferring data to cloudinary for this field (e.g. alt, caption etc)"),
  );

  $form['cloudinary_context_mapping_settings_form_fieldset']['cloudinary_add_to_field_mapping_fieldset']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add/Update mapping'),
  );

  $form['#submit'][] = 'cloudinary_context_mapping_settings_form_submit';

  return $form;
}

function cloudinary_context_mapping_settings_form_submit($form, &$form_state) {

  if ($form_state['values']['op'] == 'Add/Update mapping') {
    // Current configuration
    $context_mapping = variable_get('cloudinary_field_mappings', array(
      "field_file_image_alt_text" => "alt",
      "field_file_image_title_text" => "caption"
    ));

    $file_entity_image_field = $form_state['values']['file_entity_image_field'];
    $cloudinary_context_key = $form_state['values']['cloudinary_context_key'];

    // If you set an empty context key and assign it to a field, remove the field
    if (strlen($cloudinary_context_key) == 0 && !empty($context_mapping[$file_entity_image_field])) {
      unset($context_mapping[$file_entity_image_field]);
    }
    // Else, add the mapping
    else {
      $context_mapping[$file_entity_image_field] = $cloudinary_context_key;
    }

    variable_set('cloudinary_field_mappings', $context_mapping);
    drupal_set_message(t('The context field mapping configuration has been saved.'));
  }
  if ($form_state['values']['op'] == 'Clear all mappings') {
    variable_set('cloudinary_field_mappings', array());
    entity_info_cache_clear();
    drupal_set_message(t('The context field mapping configuration has been cleared completely.'));
  }

}

function _cloudinary_retrieve_file_entity_image_fields() {
  $all_fields = field_info_fields();
  $file_entity_image_fields = array();
  foreach ($all_fields as $machine_name => $field_info) {
    $bundles = array_keys($field_info['bundles']);
    if (in_array('file', $bundles) && in_array('image', $field_info['bundles']['file'])) {
      $file_entity_image_fields[] = $machine_name;
    }
  }

  return $file_entity_image_fields;
}

function cloudinary_folder_settings_form($form, &$form_state) {

  $form['cloudinary_folder_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cloudinary Folder Field Mapping Setting'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t("A field mappings that will be used to extract the values of a field during upload to set the folder of the image in Cloudinary"),
  );

  $file_entity_image_fields = _cloudinary_retrieve_file_entity_image_fields();
  array_unshift($file_entity_image_fields, '');
  $file_entity_image_fields = drupal_map_assoc($file_entity_image_fields);

  $form['cloudinary_folder_fieldset']['cloudinary_folder'] = array(
    '#type' => 'select',
    '#title' => t('Folder'),
    '#options' => $file_entity_image_fields,
    '#default_value' => variable_get('cloudinary_folder', ''),
    '#description' => t("Field that designates the Folder to upload the image to."),
  );

  $form['cloudinary_folder_fieldset']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save folder configuration'),
  );

  $form['#submit'][] = 'cloudinary_folder_settings_form_submit';
  return $form;
}

function cloudinary_folder_settings_form_submit($form, &$form_state) {

  variable_set('cloudinary_folder', $form_state['values']['cloudinary_folder']);

  drupal_set_message(t('The folder configuration has been saved.'));
}

function cloudinary_batch_processing_form($form, &$form_state) {

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Batch process Images into Cloudinary'),
  );
  $form['#submit'][] = 'cloudinary_batch_processing_form_submit';
  return $form;
}

function cloudinary_batch_processing_form_submit($form, &$form_state) {

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'file')
    ->entityCondition('bundle', 'image');
  $result = $query->execute();

  if (isset($result['file'])) {
    $file_ids = array_keys($result['file']);

    $packets = _cloudinary_parcel_into_packets($file_ids, 5);

    _cloudinary_perform_batch_update($packets, count($file_ids));
    drupal_set_message(t('Batch processing complete.'));
  }
}

function cloudinary_import_form($form, &$form_state) {

  $form['cloudinary_public_id'] = array(
    '#type' => 'textfield',
    '#default_value' => '',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import data from Cloudinary.'),
  );

  return $form;
}

