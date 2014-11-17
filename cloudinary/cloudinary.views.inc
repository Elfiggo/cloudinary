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
      if (!empty($handlers[$handler_key])) {
        $data[$prefix_plus_entity_type][$view_property_key][$handler_key] = array(
          'handler' => $handler
        );
      }
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
  $data[$prefix_plus_entity_type]['preview']['field']['handler'] = 'CloudinaryEntityViewsHandlerFieldPreview';
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

    // Query plugin.
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
    // Pager plugin.
    'pager' => array(
        'MediaCloundaryBrowserPagerPlugin' => array(
          'title' => t('Display Cloudinary items'),
          'help' => t("Display Cloudinary items that this view might find"),
          'handler' => 'MediaCloundaryBrowserPagerPlugin',
          'help topic' => 'pager-none',
          'uses options' => TRUE,
          'type' => 'basic',
        ),
    ),
  );

  return $plugins;
}



class CloudinaryViewsQueryPlugin extends views_plugin_query_default {

  function init($base_table, $base_field, $options) {
    parent::init($base_table, $base_field, $options);

    $table = views_fetch_data($base_table);
    
    $this->entityType = $table['table']['entity type'];
  }
  
    /**
   * Builds the necessary info to execute the query.
   */
  function build(&$view) {
      parent::build($view);
    // Store the view in the object to be able to use it later.
    $this->view = $view;

    $view->init_pager();

    // Let the pager modify the query to add limits.
    $this->pager->query();
  }
  
  function execute(&$view) {

    $start = microtime(true);
    
    $query = $view->build_info['query'];

    $cloudinary_context_parameters = $this->_extract_cloudinary_context_parameters($view->exposed_input);
    $cloudinary_parameters = $this->_gather_cloudinary_admin_api_parameters($view->exposed_input);
    
    // Extract parameter for number of results
    // Extract parameter for next_cursor
    
    if (empty($view->exposed_input['next_cursor'])) $this->pager->first_request = false;

    //$cloudinary_parameters['max_results'] = 3;
//    if (!empty($view->exposed_input['next_cursor'])) $pager->next_cursor = $view->exposed_input['next_cursor'];

    $token = '';

    // Pager specific code
    $this->pager->pre_execute($query);
    
    if (!empty($query->max_results)) $cloudinary_parameters['max_results'] = $query->max_results;

    $entities = $this->_query_cloudinary_admin_api($cloudinary_parameters, $cloudinary_context_parameters, $token);

    $this->pager->next_cursor = $token;
    
    // Just load all entities.
    $view->result = $entities;
    
    // Pager specific code
    $this->pager->post_execute($view->result);
    
    $view->total_rows = count($view->result);
    $view->execute_time = microtime(true) - $start;
    $view->use_ajax = true;
  }
  
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
  
  function _query_cloudinary_admin_api($cloudinary_parameters, $cloudinary_context_parameters, &$token) {

    _cloudinary_initialize();

    $api = new \Cloudinary\Api();
    
    $results = $api->resources($cloudinary_parameters);

    $results_array = get_object_vars($results);
    
    if (!empty($results_array['next_cursor'])) $token = $results_array['next_cursor'];
    
    $browsable_results = $results_array['resources'];

    foreach ($browsable_results as $cloudinary_image_array) {

      $cloudinary_image_object = new stdClass();
      foreach ($cloudinary_image_array as $key => $value) {
        $cloudinary_image_object->$key = $value;
      }
      
      $cloudinary_image_object->preview = $cloudinary_image_object->public_id;

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

/**
 * Plugin for views without pagers.
 *
 * @ingroup views_pager_plugins
 */
class MediaCloundaryBrowserPagerPlugin extends views_plugin_pager {

  var $first_request = true;
  var $next_cursor = '';
  
  function summary_title() {
    return format_plural($this->options['max_results'], '@count item', 'Paged, @count items', array('@count' => $this->options['max_results']));
  }
    
  function option_definition() {
    $options = parent::option_definition();
    $options['max_results'] = array('default' => 10);
    $options['next_cursor'] = array('default' => '');
    $options['tags'] = array(
      'contains' => array(
        'first' => array('default' => '« first', 'translatable' => TRUE),
        'next' => array('default' => 'next ›', 'translatable' => TRUE),
      ),
    );
    return $options;
  }
  
  /**
   * Provide the default form for setting options.
   */
  function options_form(&$form, &$form_state) {

    parent::options_form($form, $form_state);
    $pager_text = $this->display->handler->get_pager_text();

    $form['max_results'] = array(
      '#title' => $pager_text['items per page title'],
      '#type' => 'textfield',
      '#description' => $pager_text['items per page description'],
      '#default_value' => $this->options['max_results'],
    );

    $form['tags'] = array (
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#tree' => TRUE,
      '#title' => t('Tags'),
      '#input' => TRUE,
      '#description' => t('A lists of labels for the controls in the pager'),
    );

    $form['tags']['first'] = array(
      '#type' => 'textfield',
      '#title' => t('Text for "first"-link'),
      '#description' => t('Text for "first"-link'),
      '#default_value' => $this->options['tags']['first'],
    );

    $form['tags']['next'] = array(
      '#type' => 'textfield',
      '#title' => t('Text for "next"-link'),
      '#description' => t('Text for "next"-link'),
      '#default_value' => $this->options['tags']['next'],
    );
  }
  
  function query() {
      if (!empty($this->options['items_per_page'])) $this->query->max_results = $this->options['items_per_page'];
  }
  
  function render($input) {
      
    // The 0, 1, 3, 4 index are correct. See theme_pager documentation.
    $tags = array(
      0 => $this->options['tags']['first'],
      1 => $this->options['tags']['next'],
    );
    $variables = array(
      'tags' => $tags,
      'first_request' => $this->first_request,
      'next_cursor' => $this->next_cursor,
      'element' => $this->get_pager_id(),
      'parameters' => $input,
    );
    $output = theme_cloudinary_pager($variables);
    return $output;
  }

  function pre_execute(&$query) {
    $query->max_results = $this->options['max_results'];
  }
  function post_execute(&$result) {
  }
}

/**
 * Returns HTML for a query pager.
 *
 * Menu callbacks that display paged query results should call theme('pager') to
 * retrieve a pager control so that users can view other results. Format a list
 * of nearby pages with additional query results.
 *
 * @param $variables
 *   An associative array containing:
 *   - tags: An array of labels for the controls in the pager.
 *   - element: An optional integer to distinguish between multiple pagers on
 *     one page.
 *   - parameters: An associative array of query string parameters to append to
 *     the pager links.
 *   - quantity: The number of pages in the list.
 *
 * @ingroup themeable
 */
function theme_cloudinary_pager($variables) {
    
    $tags = $variables['tags'];

    $first_request = $variables['first_request'];
    $next_cursor = $variables['next_cursor'];

    $element = $variables['element'];
    $parameters = $variables['parameters'];

    $items = array();

    $li_first = '<a' . drupal_attributes(array('href' => url($_GET['q']))) . '>' . check_plain($tags[0]) . '</a>';
    $li_next = '<a' . drupal_attributes(array('href' => url($_GET['q'], array('query' => array('next_cursor' => $next_cursor))))) . '>' . check_plain($tags[1]) . '</a>';

    if ($first_request) {
        // Add item link that has no token
        $items[] = array(
          'class' => array('pager-first'),
          'data' => $li_first,
        );
    }
    if (strlen($next_cursor) > 0) {
      // Add item link with token
      $items[] = array(
        'class' => array('pager-first'),
        'data' => $li_next,
      );
    }
    
    if (count($items) > 0) {
    
        return '<h2 class="element-invisible">' . t('Pages') . '</h2>' . theme('item_list', array(
          'items' => $items,
          'attributes' => array('class' => array('pager')),
        ));
    }
}

class CloudinaryEntityViewsHandlerFieldPreview extends entity_views_handler_field_text {
  /**
   * Render the field.
   *
   * @param $values
   *   The values retrieved from the database.
   */
  public function render($values) {
    $response = parent::render($values);
    $image = theme_image_style_cloudinary(
       array(
         'style_name' => 'media_thumbnail',
         'cloudinary_id' => $response,
       )
    );
    return $image;
  }
}