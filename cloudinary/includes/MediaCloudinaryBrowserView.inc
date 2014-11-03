<?php

include_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'cloudinary') . '/cloudinary.admin.inc';

/**
 * @file
 * Definition of MediaBrowserView.
 */

/**
 * Media browser plugin for displaying a specific view and display.
 */
class MediaCloudinaryBrowserView extends MediaBrowserPlugin {
  /**
   * The view object from views_get_view() for this plugin.
   *
   * @var view
   */
  protected $view;

  /**
   * Implements MediaBrowserPluginInterface::__construct().
   */
  public function __construct($info, $params) {
    parent::__construct($info, $params);

    // Set up the view object with the proper display.
    if ($view = views_get_view($info['view_name'])) {
      $display_id = !empty($info['view_display_id']) ? $info['view_display_id'] : NULL;
      if ($view->set_display($display_id)) {
        $this->view = $view;
      }
    }
  }

  /**
   * Implements MediaBrowserPluginInterface::access().
   */
  public function access($account = NULL) {
    return !empty($this->view) && $this->view->access($this->view->current_display, $account);
  }

  /**
   * Implements MediaBrowserPlugin::view().
   */
  public function view() {
    
    if (!empty($this->view)) {
        
      //module_load_include('inc', 'file_entity', 'file_entity.pages');
      
      $build['MediaBrowserTab']['#tree'] = true;
      $build['MediaBrowserTab']['#prefix'] = '<div class="mediacloudinarybrowserviewcontainer">';
      $build['MediaBrowserTab']['#suffix'] = '</div>';
      $build['MediaBrowserTab']['mediacloudinarybrowserview']['#markup'] = $this->view->preview();

      //Put microform in here
      
      $build['MediaBrowserTab']['form'] = drupal_get_form('cloudinary_import_form');

      // Allow the View title to override the plugin title.
      if ($title = $this->view->get_title()) {
        $build['#title'] = $title;
      }
      $build['MediaBrowserTab']['form']['#attached']['js'][] = drupal_get_path('module', 'cloudinary') . '/js/media-cloudinary.browser.js';

      global $base_root;

      $settings = array(
        'cloudinary' => array(
          'base_root' => $base_root,
        ),
      );

      $build['MediaBrowserTab']['form']['#attached']['js'][] = array(
        'data' => $settings,
        'type' => 'setting'
      );


      return $build;
    }
  }
}