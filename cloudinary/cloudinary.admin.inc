<?php

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
    '#collapsed' => FALSE,
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