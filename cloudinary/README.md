
Welcome to Cloudinary Module for Drupal 7.


Installation:

Place the entirety of this directory in sites/all/modules/cloudinary
You must also install the following modules to use Cloudinary :

file_entity, media, media_internet, views, image, entity

Navigate to administer >> build >> modules. Enable Cloudinary.

Navigate to configuration >> Media >> Cloudinary management.

Under Cloudinary API Settings, enter you API Key, API Secret Key and Cloud Name.

You can find your details at https://cloudinary.com/console .

Under Cloudinary Context Field Mapping Settings you will find a series of
contextual field mappings, which are used to extract information from files
before uploading. The default are "field_file_image_alt_text" for "alt" text and
"field_file_image_title_text" for "caption", the two fields which are on images
by default. If you would like to add additional mappings to extract further data,
or update or delete current mappings, select the Field Machine Name of the field
you'd like to update and change the Cloudinary Context Key, alternatively set it
empty to remove the mapping for this Field. Caveat : this
only supports and text field types at this time.

Under Cloudinary Folder Field Mapping Setting you fill find an option to select
which field to use for the Folder property when uploading an image. Caveat : this
only supports taxonomy terms and text field types at this time.


Configuration / Use

During a file upload, if Cloudinary is configured correctly, images will
automatically be uploaded to Cloudinary and metadata as configured will be
extracted and uploaded as well.

These images can be viewed within Content >> Files or within Cloudinary at
https://cloudinary.com/console/media_library

Clicking on an image in cloudinary will open the image, and clicking the 
"Edit meta data" link at the top will show any metadata that was uploaded as part
of the image upload.

Although we can confirm that images have been succesfully uploaded to Cloudinary,
we still need to be able to use the images within Drupal. Uploading the images via
the Cloudinary module in Drupal has uploaded the image and stored necessary information
in a file_entity created during upload, complete with all the original metadata,
however to use it within Drupal will require the configuration of Cloudinary specific
Images Styles and the Cloudinary Field and File Formatters.


Configuring Cloudinary specific Image Styles

Cloudinary Image Styles work in the same way as regular Drupal Image Styles, and
use the same interface, but upon adding Drupal Effects, a Cloudinary Effect
should be used instead. Cloudinary Effects have a lot of additional parameters /
options for functionality that Cloudinary provides. They should be used exclusively
and the regular Image Styles should not be used. Once an Image Style has been created
with a selection of Cloudinary Image effects, it should be saved and will now be
available for Cloudinary Field and File Formatters to use.


Configuring the Cloudinary File Formatter

The Cloudinary File Formatter is used to ensure that Cloudinary images appear
correctly within the Media Browser, within Drupal itself in the form of
thumbnails etc. This is configured within
Structure >> File Types >> Image >> Manage File Display, where for each File Display,
the only displays to be enabled should be Cloudinary Image, with a Cloudinary Image
Style selected.

Images hosted on Cloudinary should now show as thumbnails throughout Drupal where
appropriate.


Configuring the Cloudinary Field Formatter

The Cloudinary Field Formatter applies to Image fields to display via Cloudinary.
These are configured on a per Bundle (or just Entity), per Field basis.

For example, go into any Display Settings form for a Content Type, by going to
Structure >> Content types >> Article >> Manage Display. Next to an field, select
from Format the Cloudinary Image Formatter, and click the cog to the far right of
it to configure it. Select a Cloudinary specific Image style and click Update.
Finally, click save at the bottom to finish configuring this field. An Image as
hosted on Cloudinary should now be rendered via the formatter when viewing this
piece of content.