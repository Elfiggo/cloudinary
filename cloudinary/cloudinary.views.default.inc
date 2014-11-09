<?php

function cloudinary_views_default_views() {

    /**
     * @file
     * The default view for the cloudinary media browser library tab.
     */

    $view = new view();
    $view->name = 'list_of_cloudinary_files';
    $view->description = '';
    $view->tag = 'default';
    $view->base_table = 'entity_cloudinary_admin_api';
    $view->human_name = 'List of cloudinary files';
    $view->core = 7;
    $view->api_version = '3.0';
    $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

    /* Display: Master */
    $handler = $view->new_display('default', 'Master', 'default');
    $handler->display->display_options['title'] = 'List of Cloudinary files';
    $handler->display->display_options['use_more_always'] = FALSE;
    $handler->display->display_options['access']['type'] = 'role';
    $handler->display->display_options['cache']['type'] = 'none';
    $handler->display->display_options['query']['type'] = 'views_query';
    $handler->display->display_options['exposed_form']['type'] = 'basic';
    $handler->display->display_options['pager']['type'] = 'MediaCloundaryBrowserPagerPlugin';
    $handler->display->display_options['pager']['options']['max_results'] = '10';
    $handler->display->display_options['style_plugin'] = 'table';
    $handler->display->display_options['style_options']['columns'] = array(
      'public_id' => 'public_id',
      'cloudinary_context_caption' => 'cloudinary_context_caption',
      'cloudinary_context_alt' => 'cloudinary_context_alt',
    );
    $handler->display->display_options['style_options']['default'] = '-1';
    $handler->display->display_options['style_options']['info'] = array(
      'public_id' => array(
        'sortable' => 0,
        'default_sort_order' => 'asc',
        'align' => '',
        'separator' => '',
        'empty_column' => 0,
      ),
      'cloudinary_context_caption' => array(
        'sortable' => 0,
        'default_sort_order' => 'asc',
        'align' => '',
        'separator' => '',
        'empty_column' => 0,
      ),
      'cloudinary_context_alt' => array(
        'sortable' => 0,
        'default_sort_order' => 'asc',
        'align' => '',
        'separator' => '',
        'empty_column' => 0,
      ),
    );
    /* Field: Cloudinary Admin API Entity: Preview */
    $handler->display->display_options['fields']['preview']['id'] = 'preview';
    $handler->display->display_options['fields']['preview']['table'] = 'entity_cloudinary_admin_api';
    $handler->display->display_options['fields']['preview']['field'] = 'preview';
    $handler->display->display_options['fields']['preview']['link_to_entity'] = 0;
    /* Field: Cloudinary Admin API Entity: Public ID */
    $handler->display->display_options['fields']['public_id']['id'] = 'public_id';
    $handler->display->display_options['fields']['public_id']['table'] = 'entity_cloudinary_admin_api';
    $handler->display->display_options['fields']['public_id']['field'] = 'public_id';
    /* Field: Cloudinary Admin API Entity: Cloudinary Context - caption */
    $handler->display->display_options['fields']['cloudinary_context_caption']['id'] = 'cloudinary_context_caption';
    $handler->display->display_options['fields']['cloudinary_context_caption']['table'] = 'entity_cloudinary_admin_api';
    $handler->display->display_options['fields']['cloudinary_context_caption']['field'] = 'cloudinary_context_caption';
    $handler->display->display_options['fields']['cloudinary_context_caption']['link_to_entity'] = 0;
    /* Field: Cloudinary Admin API Entity: Cloudinary Context - alt */
    $handler->display->display_options['fields']['cloudinary_context_alt']['id'] = 'cloudinary_context_alt';
    $handler->display->display_options['fields']['cloudinary_context_alt']['table'] = 'entity_cloudinary_admin_api';
    $handler->display->display_options['fields']['cloudinary_context_alt']['field'] = 'cloudinary_context_alt';
    $handler->display->display_options['fields']['cloudinary_context_alt']['link_to_entity'] = 0;
    /* Field: Cloudinary Admin API Entity: Format */
    $handler->display->display_options['fields']['format']['id'] = 'format';
    $handler->display->display_options['fields']['format']['table'] = 'entity_cloudinary_admin_api';
    $handler->display->display_options['fields']['format']['field'] = 'format';
    $handler->display->display_options['fields']['format']['link_to_entity'] = 0;
    /* Field: Cloudinary Admin API Entity: Cloudinary Context - credit */
    $handler->display->display_options['fields']['cloudinary_context_credit']['id'] = 'cloudinary_context_credit';
    $handler->display->display_options['fields']['cloudinary_context_credit']['table'] = 'entity_cloudinary_admin_api';
    $handler->display->display_options['fields']['cloudinary_context_credit']['field'] = 'cloudinary_context_credit';
    $handler->display->display_options['fields']['cloudinary_context_credit']['link_to_entity'] = 0;
    /* Filter criterion: Cloudinary Admin API Entity: Prefix to Public ID */
    $handler->display->display_options['filters']['prefix']['id'] = 'prefix';
    $handler->display->display_options['filters']['prefix']['table'] = 'entity_cloudinary_admin_api';
    $handler->display->display_options['filters']['prefix']['field'] = 'prefix';
    $handler->display->display_options['filters']['prefix']['group'] = 1;
    $handler->display->display_options['filters']['prefix']['exposed'] = TRUE;
    $handler->display->display_options['filters']['prefix']['expose']['operator_id'] = 'prefix_op';
    $handler->display->display_options['filters']['prefix']['expose']['label'] = 'Prefix to Public ID';
    $handler->display->display_options['filters']['prefix']['expose']['operator'] = 'prefix_op';
    $handler->display->display_options['filters']['prefix']['expose']['identifier'] = 'prefix';
    $handler->display->display_options['filters']['prefix']['expose']['remember'] = TRUE;
    $handler->display->display_options['filters']['prefix']['expose']['remember_roles'] = array(
      2 => '2',
      1 => 0,
      3 => 0,
    );
    /* Filter criterion: Cloudinary Admin API Entity: Folder */
    $handler->display->display_options['filters']['folder']['id'] = 'folder';
    $handler->display->display_options['filters']['folder']['table'] = 'entity_cloudinary_admin_api';
    $handler->display->display_options['filters']['folder']['field'] = 'folder';
    $handler->display->display_options['filters']['folder']['group'] = 1;
    $handler->display->display_options['filters']['folder']['exposed'] = TRUE;
    $handler->display->display_options['filters']['folder']['expose']['operator_id'] = 'folder_op';
    $handler->display->display_options['filters']['folder']['expose']['label'] = 'Folder';
    $handler->display->display_options['filters']['folder']['expose']['operator'] = 'folder_op';
    $handler->display->display_options['filters']['folder']['expose']['identifier'] = 'folder';
    $handler->display->display_options['filters']['folder']['expose']['remember_roles'] = array(
      2 => '2',
      1 => 0,
      3 => 0,
    );
    /* Filter criterion: Cloudinary Admin API Entity: Public ID */
    $handler->display->display_options['filters']['public_id']['id'] = 'public_id';
    $handler->display->display_options['filters']['public_id']['table'] = 'entity_cloudinary_admin_api';
    $handler->display->display_options['filters']['public_id']['field'] = 'public_id';
    $handler->display->display_options['filters']['public_id']['group'] = 1;
    $handler->display->display_options['filters']['public_id']['exposed'] = TRUE;
    $handler->display->display_options['filters']['public_id']['expose']['operator_id'] = 'public_id_op';
    $handler->display->display_options['filters']['public_id']['expose']['label'] = 'Public ID';
    $handler->display->display_options['filters']['public_id']['expose']['operator'] = 'public_id_op';
    $handler->display->display_options['filters']['public_id']['expose']['identifier'] = 'public_id';
    $handler->display->display_options['filters']['public_id']['expose']['remember_roles'] = array(
      2 => '2',
      1 => 0,
      3 => 0,
    );
    /* Filter criterion: Cloudinary Admin API Entity: Cloudinary Context - caption */
    $handler->display->display_options['filters']['cloudinary_context_caption']['id'] = 'cloudinary_context_caption';
    $handler->display->display_options['filters']['cloudinary_context_caption']['table'] = 'entity_cloudinary_admin_api';
    $handler->display->display_options['filters']['cloudinary_context_caption']['field'] = 'cloudinary_context_caption';
    $handler->display->display_options['filters']['cloudinary_context_caption']['group'] = 1;
    $handler->display->display_options['filters']['cloudinary_context_caption']['exposed'] = TRUE;
    $handler->display->display_options['filters']['cloudinary_context_caption']['expose']['operator_id'] = 'cloudinary_context_caption_op';
    $handler->display->display_options['filters']['cloudinary_context_caption']['expose']['label'] = 'Cloudinary Context - caption';
    $handler->display->display_options['filters']['cloudinary_context_caption']['expose']['operator'] = 'cloudinary_context_caption_op';
    $handler->display->display_options['filters']['cloudinary_context_caption']['expose']['identifier'] = 'cloudinary_context_caption';
    $handler->display->display_options['filters']['cloudinary_context_caption']['expose']['remember_roles'] = array(
      2 => '2',
      1 => 0,
      3 => 0,
    );
    /* Filter criterion: Cloudinary Admin API Entity: Cloudinary Context - alt */
    $handler->display->display_options['filters']['cloudinary_context_alt']['id'] = 'cloudinary_context_alt';
    $handler->display->display_options['filters']['cloudinary_context_alt']['table'] = 'entity_cloudinary_admin_api';
    $handler->display->display_options['filters']['cloudinary_context_alt']['field'] = 'cloudinary_context_alt';
    $handler->display->display_options['filters']['cloudinary_context_alt']['group'] = 1;
    $handler->display->display_options['filters']['cloudinary_context_alt']['exposed'] = TRUE;
    $handler->display->display_options['filters']['cloudinary_context_alt']['expose']['operator_id'] = 'cloudinary_context_alt_op';
    $handler->display->display_options['filters']['cloudinary_context_alt']['expose']['label'] = 'Cloudinary Context - alt';
    $handler->display->display_options['filters']['cloudinary_context_alt']['expose']['operator'] = 'cloudinary_context_alt_op';
    $handler->display->display_options['filters']['cloudinary_context_alt']['expose']['identifier'] = 'cloudinary_context_alt';
    $handler->display->display_options['filters']['cloudinary_context_alt']['expose']['remember_roles'] = array(
      2 => '2',
      1 => 0,
      3 => 0,
    );
    /* Filter criterion: Cloudinary Admin API Entity: Cloudinary Context - credit */
    $handler->display->display_options['filters']['cloudinary_context_credit']['id'] = 'cloudinary_context_credit';
    $handler->display->display_options['filters']['cloudinary_context_credit']['table'] = 'entity_cloudinary_admin_api';
    $handler->display->display_options['filters']['cloudinary_context_credit']['field'] = 'cloudinary_context_credit';
    $handler->display->display_options['filters']['cloudinary_context_credit']['exposed'] = TRUE;
    $handler->display->display_options['filters']['cloudinary_context_credit']['expose']['operator_id'] = 'cloudinary_context_credit_op';
    $handler->display->display_options['filters']['cloudinary_context_credit']['expose']['label'] = 'Cloudinary Context - credit';
    $handler->display->display_options['filters']['cloudinary_context_credit']['expose']['operator'] = 'cloudinary_context_credit_op';
    $handler->display->display_options['filters']['cloudinary_context_credit']['expose']['identifier'] = 'cloudinary_context_credit';
    $handler->display->display_options['filters']['cloudinary_context_credit']['expose']['remember_roles'] = array(
      2 => '2',
      1 => 0,
      3 => 0,
    );
    /* Filter criterion: Cloudinary Admin API Entity: Width */
    $handler->display->display_options['filters']['width']['id'] = 'width';
    $handler->display->display_options['filters']['width']['table'] = 'entity_cloudinary_admin_api';
    $handler->display->display_options['filters']['width']['field'] = 'width';
    $handler->display->display_options['filters']['width']['exposed'] = TRUE;
    $handler->display->display_options['filters']['width']['expose']['operator_id'] = 'width_op';
    $handler->display->display_options['filters']['width']['expose']['label'] = 'Width';
    $handler->display->display_options['filters']['width']['expose']['operator'] = 'width_op';
    $handler->display->display_options['filters']['width']['expose']['identifier'] = 'width';
    $handler->display->display_options['filters']['width']['expose']['remember_roles'] = array(
      2 => '2',
      1 => 0,
      3 => 0,
    );

    /* Display: Page */
    $handler = $view->new_display('page', 'Page', 'page');
    $handler->display->display_options['path'] = 'list-of-cloudinary-files';

    /* Display: Cloudinary Media browser tab */
    $handler = $view->new_display('MediaCloudinaryBrowserDisplayPlugin', 'Cloudinary Media browser tab', 'MediaCloudinaryBrowserDisplayPlugin_1');
    
    $views[$view->name] = $view;
    
    return $views;
}