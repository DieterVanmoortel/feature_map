(function($) {

  /*  Preview image */
  Drupal.behaviors.GmapLocator = {
    attach: function(context) {
      /* Initialise google maps */
      gmap_init();

      /* address lookup */
      $('#zipcodesearchform .location-submit').bind('click', function(e){
        e.preventDefault();
        var address = $('#zipcodesearchform #location').val();
        lookupAddress(address);
      });

      /* Filtering */
      
      $('.triggerdetails').bind('click', function(e){
        $('.infodetails').toggle();
      })
    }
  }

  function gmap_init(){
    if (Drupal.settings.feature_map.lat && Drupal.settings.feature_map.lng) {
      set_preview_marker();
    }
    else{    
      load_markers();
    }
  }

  function set_preview_marker() {
    var myLatlng = new google.maps.LatLng(Drupal.settings.feature_map.lat,Drupal.settings.feature_map.lng);
    var myOptions = {
      zoom: 8,
      center: myLatlng,
      mapTypeId: google.maps.MapTypeId.ROADMAP,
      disableDefaultUI: true
    }
    var map = new google.maps.Map(document.getElementById("gmap"), myOptions);

    var marker = new google.maps.Marker({
      position: myLatlng,
      map: map
    });
  }
  function load_markers(markers) {

    // don't allow simultaneous loads
    try{xhr.abort();}
    catch(err){}
    
    if(typeof(markers) == 'undefined'){
      var markers = 'initial';
    }
    
    xhr = $.ajax({
      type: 'POST',
      url: Drupal.settings.feature_map.ajaxPath,
      data: {
        markers: markers
      },
      success : function(data) {
        var myOptions = {
          scrollwheel: false,
          zoom: 7,
          center: get_default_location(), 
          mapTypeId: google.maps.MapTypeId.TERRAIN
        };
        map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);
        $.each(data, function(pos, data) {
          var marker = new google.maps.Marker({
              position: new google.maps.LatLng(data.lat,data.lng),
              icon: "/sites/all/modules/dev/feature_map/theme/redmarker.png",
              html: data.teaser,
              id: pos,
              map: map
            });
            
            // To add the marker to the map, call setMap();
          infoBubble = new InfoBubble ({
            shadowStyle: 1,
            padding: 0,
            backgroundColor: 'rgb(57,57,57)',
            borderRadius: 4,
            arrowSize: 10,
            borderWidth: 1,
            borderColor: '#2c2c2c',
            disableAutoPan: true,
            hideCloseButton: true,
            arrowPosition: 60,
            backgroundClassName: 'infoBubble',
            arrowStyle: 2,
            disableAnimation:true
          });
          google.maps.event.addListener(marker, 'click', function() {
            infoBubble.setContent('<div class="infotext">' + this.html + '</div>');
            infoBubble.open(map, this);
          });
          
          $('#descriptionblock').append('<div>' + data.full+ '</div>');
        });
      }
    });
  }



  function lookupAddress(address){
    var geocoder;
    geocoder = new google.maps.Geocoder();
    geocoder.geocode( { 'address': address}, function(results, status) {
      if (status == google.maps.GeocoderStatus.OK) {
        $('#zipcodesearchform .location-error').remove();
        map.setCenter(results[0].geometry.location);
        var marker = new google.maps.Marker({
            map: map,
            title: address,
            position: results[0].geometry.location,
            icon: "/sites/all/modules/dev/feature_map/theme/greenmarker.png"
        });
      }
      else{
        $('#zipcodesearchform').append('<div class="location-error">' + Drupal.t('No results found') + '</div>');
      }
    });
  }
  function get_default_location(errorFlag) {
    initialLocation = new google.maps.LatLng(50.833, 4.333);// default to brussels
    initialLocation = new google.maps.LatLng(46.833, 6.333);// default to switserland
    var cookieLocation = getCookie("location");
    return (!cookieLocation) ? initialLocation : cookieLocation;
  }

  function getCookie(c_name) {
  var i,x,y,ARRcookies = document.cookie.split(";");
  for (i = 0; i < ARRcookies.length; i++) {
    x = ARRcookies[i].substr(0, ARRcookies[i].indexOf("="));
    y = ARRcookies[i].substr(ARRcookies[i].indexOf("=") + 1);
    x = x.replace(/^\s+|\s+$/g, "");
    if (x == c_name) {
      return unescape(y);
    }
  }
  return false;
  }
  
})(jQuery);


