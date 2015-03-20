<?php

/**
 * @file
 * Batch functionality for bulk synchronisation to cloudinary.
 */

function _cloudinary_parcel_into_packets($items, $number_per_packet = 2) {

  $packets = array();
  $current_packet = NULL;
  $items_keys = array_keys($items);

  for ($counter = 0; $counter < count($items); $counter++) {
    if ($counter % $number_per_packet == 0) {
      if ($current_packet != NULL) {
        $packets[] = $current_packet;
      }
      $current_packet = array();
    }
    $current_packet[$items_keys[$counter]] = $items[$items_keys[$counter]];
  }

  if ($current_packet != NULL) {
    $packets[] = $current_packet;
  }

  return $packets;
}

function _cloudinary_perform_batch_update($packets, $total, $drush_mode = FALSE) {

  $batch = array(
    'operations' => array(// These are set below if needed.
    ),
    'finished' => 'cloudinary_batch_item_update_finished',
    'title' => t('Cloudinary batch processing'),
    'init_message' => t('Cloudinary batch processing is starting.'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Cloudinary Batch Processing initialization has encountered an error.'),
  );

  foreach ($packets as $packet) {
    $batch['operations'][] = array(
      'cloudinary_batch_item_update_process',
      array($packet, $total)
    );
  }

  if (count($batch['operations']) > 0) {
    batch_set($batch);

    if ($drush_mode) {
      $batch = &batch_get();
      $batch['progressive'] = FALSE;
      drush_backend_batch_process();
    }
  }
}

function cloudinary_batch_item_update_process($packet, $total_number_of_items, &$context) {

  if (empty($context['sandbox']['progress'])) {

    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = $total_number_of_items;
    $context['finished'] = 0;
  }

  foreach ($packet as $file_id) {
    $file = file_load($file_id);
    file_save($file);
  }

  $message = t('Now updating items %previous - %current', array(
    '%previous' => $context['sandbox']['progress'],
    '%current' => $context['sandbox']['progress'] + count($packet)
  ));

  $context['message'] = $message;

  // Update progress
  $context['sandbox']['progress'] += count($packet);
  foreach ($packet as $item) {
    $context['results'][] = 1;
  }

  $context['finished'] = 1;
}

function cloudinary_batch_item_update_finished($success, $results, $operations) {

  if ($success) {
    // Here we do something meaningful with the results.
    $message = count($results) . ' item(s) processed.';
    drupal_set_message(check_plain($message));
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    $message = t('An error occurred while processing %error_operation with arguments: @arguments', array(
      '%error_operation' => $error_operation[0],
      '@arguments' => print_r($error_operation[1], TRUE)
    ));
    drupal_set_message($message, 'error');
  }
}

