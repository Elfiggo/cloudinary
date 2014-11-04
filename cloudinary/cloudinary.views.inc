<?php

/**
 * Implements hook_views_data_alter().
 *
 * Makes the views table provided by the entity api module usable as base table.
 */
function cloudinary_views_data_alter(&$data) {

  $entity_type = 'cloudinary_admin_api';
  $prefix_plus_entity_type = 'entity_' . $entity_type;
  $data[$prefix_plus_entity_type]['table']['base'] = array(
    'field' => 'public_id',
    'title' => 'Cloudinary Admin API',
    'help' => t('Cloudinary Admin API'),
    'query class' => 'CloudinaryViewsQueryPlugin',
  );
  $data[$prefix_plus_entity_type]['table']['entity type'] = $entity_type;
  
  foreach ($data[$prefix_plus_entity_type] as $view_property_key => $view_property_details) {
      if ($view_property_key === 'table') continue;
      
      $data[$prefix_plus_entity_type][$view_property_key]['field']['click sortable'] = TRUE;
      
      $property_type = $data[$prefix_plus_entity_type][$view_property_key]['field']['type'];
      $handlers['filter'] = _cloudinary_map_property_types_to_view_filters($property_type);
      $handlers['argument'] = _cloudinary_map_property_types_to_view_arguments($property_type);
      $handlers['sort'] = _cloudinary_map_property_types_to_view_sort($property_type);
      
      foreach ($handlers as $handler_key => $handler) {
        if (!empty($handlers[$handler_key])) $data[$prefix_plus_entity_type][$view_property_key][$handler_key] = array(
          'handler' => $handler
        );
      }
  }
  
  $data[$prefix_plus_entity_type]['prefix'] = array(
    'title' => 'Prefix to Public ID',
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  $data[$prefix_plus_entity_type]['folder'] = array(
    'title' => 'Folder',
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
}

function _cloudinary_map_property_types_to_view_filters($property_type) {
    $handler = FALSE;
    
    switch($property_type) {
        case 'text' : $handler = 'views_handler_filter_string'; break;
        case 'integer' : $handler = 'views_handler_filter_numeric'; break;
    }
    
    return $handler;
}

function _cloudinary_map_property_types_to_view_arguments($property_type) {
    $handler = FALSE;
    
    switch($property_type) {
        case 'text' : $handler = 'views_handler_argument_string'; break;
        case 'integer' : $handler = 'views_handler_argument_numeric'; break;
    }
    
    return $handler;
}

function _cloudinary_map_property_types_to_view_sort($property_type) {
    $handler = FALSE;
    
    switch($property_type) {
        case 'text' : case 'integer' : $handler = 'views_handler_sort'; break;
    }
    
    return $handler;
}

/**
 * Implements hook_views_plugins().
 *
 * Generate a list of which base-tables to enabled the plugins for.
 */
function cloudinary_views_plugins() {
    
  $base = array('entity_cloudinary_admin_api');
  $plugins = array(
    'query' => array(
      'CloudinaryViewsQueryPlugin' => array(
        'title' => t('Cloudinary Admin API Query'),
        'help' => t('Uses Cloudinary Admin API for retrieving image data.'),
        'handler' => 'CloudinaryViewsQueryPlugin',
      ),
    ),
    // Display plugin.
    'display' => array(
      'MediaCloudinaryBrowserDisplayPlugin' => array(
        'title' => t('Cloudinary Media browser tab'),
        'help' => t('Display as a tab in the media browser.'),
        'handler' => 'MediaCloudinaryBrowserDisplayPlugin',
        'theme' => 'views_view',
        'base' => $base,
        'use ajax' => TRUE,
        'use pager' => TRUE,
        'accept attachments' => TRUE,
      ),
    ),
    // Style plugin.
    'style' => array(
      'MediaCloudinaryBrowserStylePlugin' => array(
        'title' => t('Media Cloudinary Table'),
        'help' => t('Displays rows in a table with click to import file in Drupal.'),
        'handler' => 'MediaCloudinaryBrowserStylePlugin',
        'theme' => 'views_view_table',
        'base' => $base,
        'uses row plugin' => FALSE,
        'uses row class' => TRUE,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'help topic' => 'style-table',
      ),
    ),
  );

  return $plugins;
}



class CloudinaryViewsQueryPlugin extends views_plugin_query {
    
  public $entityType;
    
  function init($base_table, $base_field, $options) {
    parent::init($base_table, $base_field, $options);

    $table = views_fetch_data($base_table);
    
    $this->entityType = $table['table']['entity type'];
  }
  
  function execute(&$view) {
      
    $start = microtime(true);

    $cloudinary_context_parameters = $this->_extract_cloudinary_context_parameters($view->exposed_input);
    $cloudinary_parameters = $this->_gather_cloudinary_admin_api_parameters($view->exposed_input);

    $entities = $this->_query_cloudinary_admin_api($cloudinary_parameters, $cloudinary_context_parameters);
    
    // Just load all entities.
    $view->result = $entities;
    $view->total_rows = count($view->result);
    $view->execute_time = microtime(true) - $start;
    $view->use_ajax = true;
  }
  
  function add_where() {}
  function add_orderby() {}
  
  function _extract_cloudinary_context_parameters($input) {

    $cloudinary_unprocessed_parameters = $input;
    $cloudinary_parameters = array();

    // First, extract the mapping parameters
    $cloudinary_field_mapping_parameters = array();
    $cloudinary_field_mappings = variable_get('cloudinary_field_mappings', array());
    foreach($cloudinary_field_mappings as $field => $mapping) {

        $mapping_key = 'cloudinary_context_' . $mapping;
        if (!empty($cloudinary_unprocessed_parameters[$mapping_key])) {
            $cloudinary_field_mapping_parameters[$mapping] = $cloudinary_unprocessed_parameters[$mapping_key];
            unset($cloudinary_unprocessed_parameters[$mapping_key]);
        }
        if (isset($cloudinary_unprocessed_parameters[$mapping_key]) && strlen($cloudinary_unprocessed_parameters[$mapping_key]) == 0) {
            unset($cloudinary_unprocessed_parameters[$mapping_key]);
        }
    }

    // Add to the cloudinary parameters
    if (count($cloudinary_field_mapping_parameters) > 0) {
        foreach ($cloudinary_field_mapping_parameters as $cloudinary_field_mapping_parameter_key => $cloudinary_field_mapping_parameter_value) {
            $cloudinary_parameters[$cloudinary_field_mapping_parameter_key] = $cloudinary_field_mapping_parameter_value;
        }
    }
    return $cloudinary_parameters;
  }
  
  function _gather_cloudinary_admin_api_parameters($input) {

    // Need to gather all parameters and consolidate them into a single array to pass to admin API
    $cloudinary_unprocessed_parameters = $input;
    $removed_elements = array('live_preview', '_triggering_element_name', '_triggering_element_value', 'js');
    foreach($removed_elements as $element) unset($cloudinary_unprocessed_parameters[$element]);
    
    $prefix = !empty($cloudinary_unprocessed_parameters['prefix']) ? $cloudinary_unprocessed_parameters['prefix'] : '';
    unset($cloudinary_unprocessed_parameters['prefix']);
    
    $folder = !empty($cloudinary_unprocessed_parameters['folder']) ? $cloudinary_unprocessed_parameters['folder'] : '';
    unset($cloudinary_unprocessed_parameters['folder']);
    
    $prefix_parameters = array();
    if (strlen($prefix) > 0) $prefix_parameters[] = $prefix;
    if (strlen($folder) > 0) $prefix_parameters[] = $folder;
    $prefix_parameter_string = implode('/', $prefix_parameters);
    
    // Set up Cloudinary parameters
    $cloudinary_parameters = array(
      'context' => true,
      'type' => 'upload',
    );
    if (strlen($prefix_parameter_string) > 0) $cloudinary_parameters['prefix'] = $prefix_parameter_string;

    // Finally, extract anything left over and insert as additional parameters
    // The names of the parameters have to be exactly the same as the admin api requires as there is no mapping
    foreach ($cloudinary_unprocessed_parameters as $unprocessed_parameter_key => $unprocessed_parameter_value) {
        $cloudinary_parameters[$unprocessed_parameter_key] = $unprocessed_parameter_value;
    }

    return $cloudinary_parameters;
  }
  
  function _query_cloudinary_admin_api($cloudinary_parameters, $cloudinary_context_parameters) {

    _cloudinary_initialize();

    $api = new \Cloudinary\Api();

    $results = $api->resources($cloudinary_parameters);
    $results_array = get_object_vars($results);
    $browsable_results = $results_array['resources'];

    foreach ($browsable_results as $cloudinary_image_array) {

      $cloudinary_image_object = new stdClass();
      foreach ($cloudinary_image_array as $key => $value) {
        $cloudinary_image_object->$key = $value;
      }

      // Retrieve the resource details of this image
      // Strangely, the public id has to be urlencoded to work
      /*$image_resource = $api->resource(urlencode($cloudinary_image_object->public_id), array('image_metadata' => true));
      $image_resource_array = get_object_vars($image_resource);

      foreach ($image_resource_array as $key => $value) {
        $cloudinary_image_object->$key = $value;
      }*/

      // Retrieve the context parameters and add to the entity
      $cloudinary_field_mappings = variable_get('cloudinary_field_mappings', array());
      foreach($cloudinary_field_mappings as $field => $mapping) {
        $mapping_key = 'cloudinary_context_' . $mapping;

        if (!empty($cloudinary_image_object->context['custom'][$mapping])) {
          $cloudinary_image_object->$mapping_key = $cloudinary_image_object->context['custom'][$mapping];
        }
      }

      // Need to filter now based on contextual values
      $failed_filtering = false;
      $cloudinary_context_parameter_keys = array_keys($cloudinary_context_parameters);
      
      for ($counter = 0; $counter < count($cloudinary_context_parameter_keys) && ! $failed_filtering; $counter ++) {
          $value = $cloudinary_image_object->context['custom'][$cloudinary_context_parameter_keys[$counter]];
          $cloudinary_context_parameter_value = $cloudinary_context_parameters[$cloudinary_context_parameter_keys[$counter]];
          
          if (stripos($value, $cloudinary_context_parameter_value) !== FALSE) {}
          else {
            $failed_filtering = true;
            break;
          }
      }
      
      if (!$failed_filtering) $entities[] = $cloudinary_image_object;
    }
    
    return $entities;
  }
  
  /**
   * Gets wrappers for the result entities.
   */
  public function get_result_wrappers($results, $relationship = NULL, $field = NULL) {
    $wrappers = array();
    foreach ($results as $id => $entity) {
      $wrappers[$id] = entity_metadata_wrapper($this->entityType, $entity);
    }

     // Apply the relationship, if necessary.
    $selector_suffix = '';
    if ($field && ($pos = strrpos($field, ':'))) {
      $selector_suffix = substr($field, 0, $pos);
    }
    if ($selector_suffix || ($relationship && !empty($this->view->relationship[$relationship]))) {
      // Use EntityFieldHandlerHelper to compute the correct data selector for
      // the relationship.
      $handler = (object) array(
          'view' => $this->view,
          'relationship' => $relationship,
          'real_field' => '',
      );
      $selector = EntityFieldHandlerHelper::construct_property_selector($handler);
      $selector .= ($selector ? ':' : '') . $selector_suffix;
      return EntityFieldHandlerHelper::extract_property_multiple($wrappers, $selector);
    }

    // If no relationship is given, just return the entities.
     return array($this->entityType, $wrappers);
  }
}