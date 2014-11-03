/**
 * @file
 * Handles the JS for the views file browser. Note that this does not currently
 * support multiple file selection
 */


(function ($) {

  Drupal.behaviors.mediaCloudinaryBrowser = {
    attach: function (context, settings) {

      // Disable the links on media items list
      $('.mediacloudinarybrowserviewcontainer a').click(function() {
        return false;
      });
      
      var selectMediaCloudinaryBrowserViewContainer = '.mediacloudinarybrowserviewcontainer',
          selectAllTableRowsQuery = selectMediaCloudinaryBrowserViewContainer + ' .views-table tr',
          selectedSelectedPublicIDQuery = selectAllTableRowsQuery + '.selected .views-field-public-id',
          selectPublicIDHiddenFormElementQuery = selectMediaCloudinaryBrowserViewContainer + ' input[name=cloudinary_public_id]';
          selectSubmitButtonQuery = selectMediaCloudinaryBrowserViewContainer + ' input[name=op]',

      $(selectAllTableRowsQuery).click(function() {
          var allRows = $(selectAllTableRowsQuery);
          allRows.removeClass('selected');
          $(this).addClass('selected');
          
          var selectedRowPublicID = $(selectedSelectedPublicIDQuery);
          var public_id = selectedRowPublicID.text().trim();
          $(selectPublicIDHiddenFormElementQuery).val(public_id);
      });
      
      $(selectSubmitButtonQuery).click(function(e) {
          
          var processImportFileResult = function(data) {
            if (data.error != 0) {
                alert(data.error_message);
            }
            else {
                var generateFile = data.data;
                var files = new Array();
                files.push(generateFile);
                Drupal.media.browser.selectMediaAndSubmit(files);
            }
          };
          
          var selectedRowPublicID = $(selectPublicIDHiddenFormElementQuery).val();
          var object = {cloudinary_public_id:selectedRowPublicID};
          var data_json = JSON.stringify(object);
          var request = $.ajax({
            type: 'POST',
            url: Drupal.settings.cloudinary.base_root + '/cloudinary/create',
            data: {data:data_json},
            success : (function( data ) {
              processImportFileResult(data);
            }),
            fail : (function( jqXHR, textStatus ) {
              alert( "Request failed: " + textStatus );
            })
          });
          
          e.stopPropagation();
          e.stopImmediatePropagation();
          return false;
      });
    }
  };
  

}(jQuery));