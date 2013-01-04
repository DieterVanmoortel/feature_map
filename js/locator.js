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
        buildMap(address);
      });

      /* Filtering */
      

    }
  }

  function gmap_init(){
    // for content preview
    if (Drupal.settings.featureMap.lat && Drupal.settings.featureMap.lng) {
      var myLatlng = new google.maps.LatLng(Drupal.settings.featureMap.lat,Drupal.settings.featureMap.lng);
      var myOptions = {
        zoom: 10,
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
    // for the general map
    else{    
      buildMap();
//      load_markers(); 
    }
  }

  
  function load_markers(markers) {
    var loadedMarkers = [];
    // don't allow simultaneous loads
    try{xhr.abort();}
    catch(err){}
    
    if(typeof(markers) == 'undefined'){
      var markers = 'initial';
    }

    
    xhr = $.ajax({
      type: 'POST',
      url: Drupal.settings.featureMap.ajaxPath,
      data: {
        markers: markers
      },
      success : function(data) {
        
        $.each(data, function(pos, data) {
          var marker = new google.maps.Marker({
              position: new google.maps.LatLng(data.lat,data.lng),
              icon:  Drupal.settings.featureMap.modulePath + '/theme/redmarker.png'
            });
            // To add the marker to the map, call setMap();
          marker.setMap(map);
          loadedMarkers.push(marker);
        });
      }
    });
  }

  function buildMap(address) {
    var myOptions = {
      scrollwheel: false,
      zoom: 7,
      center: get_default_location(), 
      mapTypeId: google.maps.MapTypeId.TERRAIN
    };
    map = new google.maps.Map(document.getElementById("map_canvas"), myOptions); 
    if(typeof(address) == 'undefined'){
      var location = get_default_location();
    }
    else {
      var geocoder;
      geocoder = new google.maps.Geocoder(); 
      geocoder.geocode( { 'address': address}, function(results, status) {
      if (status == google.maps.GeocoderStatus.OK) {
        var location = results[0].geometry.location;
        $('#zipcodesearchform .location-error').remove();
        map.setCenter(location);
        setLocationCookie(location);
      }
      else{
        $('#zipcodesearchform').append('<div class="location-error">' + Drupal.t('No results found') + '</div>');
      }
      });
    }
    console.log('test');
    var marker = new google.maps.Marker({
      map: map,
      position: location,
      icon: Drupal.settings.featureMap.modulePath + '/theme/blackmarker.png'
    });   
    marker.setMap(map);
  }
  function get_default_location() {
    var location = new google.maps.LatLng(50.833, 4.333);// default to brussels
    var cookieLocation = getCookie("location");
    if(cookieLocation) {
      cookieLocation = cookieLocation.replace('(', '').replace(')', '').split(',');
      location = new google.maps.LatLng(parseFloat(cookieLocation[0]), parseFloat(cookieLocation[1]));
    }
    return location;
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
  
  function setLocationCookie(locationObject) {
    setCookie('location', locationObject.toString());
  }
  
  function setCookie(c_name,value,exdays) {
    var exdate=new Date();
    exdate.setDate(exdate.getDate() + exdays);
    var c_value=escape(value) + ((exdays==null) ? "" : "; expires="+exdate.toUTCString());
    document.cookie=c_name + "=" + c_value;
  }

})(jQuery);


