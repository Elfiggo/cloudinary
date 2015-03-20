<?php

/**
 * @file
 *  Create a Vimeo Stream Wrapper class for the Media/Resource module.
 */

/**
 *  Create an instance like this:
 *  $cloudinary = new MediaCloudinaryStreamWrapper('cloudinary://id/[cloudinary-id]/ext/[original-file-extension]/original/[original-uri]');
 */
class MediaCloudinaryStreamWrapper extends MediaReadOnlyStreamWrapper {
  static function getMimeType($uri, $mapping = NULL) {
    return 'image/png';
  }

  static function encodeURI($cloudinary_id, $extension, $original_image_uri = NULL) {

    $original_image_parameter = '';
    if ($original_image_uri != NULL) {
      $original_image_parameter = sprintf('original/%s', urlencode($original_image_uri));
    }
    return sprintf('cloudinary://id/%s/ext/%s/%s', urlencode($cloudinary_id), urlencode($extension), $original_image_parameter);
  }

  public function get_original_image_uri() {
    $unprocessed_parameters = $this->parameters;
    if (!empty($parameters['original'])) {
      return urldecode($parameters['original']);
    }
    else {
      return FALSE;
    }
  }

  public function get_parameters() {

    $unprocessed_parameters = $this->parameters;

    $processed_parameters = array();
    $processed_parameters['id'] = urldecode($unprocessed_parameters['id']);
    $processed_parameters['ext'] = urldecode($unprocessed_parameters['ext']);
    if (!empty($unprocessed_parameters['original'])) {
      $processed_parameters['original'] = urldecode($unprocessed_parameters['original']);
    }

    return $processed_parameters;
  }

  function getTarget($f) {
    return FALSE;
  }

  function interpolateUrl() {
    $parameters = $this->get_parameters();

    $image = array(
      'cloudinary_id' => $parameters['id'],
      'extension' => $parameters['ext'],
    );
    return theme('image_src_cloudinary', $image);
  }

  function getOriginalStyled($style) {
    $parameters = $this->get_parameters();
    $image = array(
      'style_name' => $style,
      'cloudinary_id' => $parameters['id'],
      'extension' => $parameters['ext'],
    );
    return theme('image_style_cloudinary', $image);
  }

  function getOriginalThumbnailStylePath($style) {
    $parameters = $this->get_parameters();
    $image = array(
      'style_name' => $style,
      'cloudinary_id' => $parameters['id'],
      'extension' => $parameters['ext'],
    );
    return theme('image_style_src_cloudinary', $image);
  }

  function getOriginalThumbnailPath() {
    $parameters = $this->get_parameters();
    $image = array(
      'style_name' => 'thumbnail',
      'cloudinary_id' => $parameters['id'],
      'extension' => $parameters['ext'],
    );
    return theme('image_style_src_cloudinary', $image);
  }

  function getLocalThumbnailPath() {
    $parameters = $this->get_parameters();

    $local_path = 'public://cloudinary/' . $parameters['id'] . '.' . $parameters['ext'];
    if (!file_exists($local_path)) {
      $dirname = drupal_dirname($local_path);
      file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
      @copy($this->getOriginalThumbnailPath(), $local_path);
    }
    return $local_path;
  }
}