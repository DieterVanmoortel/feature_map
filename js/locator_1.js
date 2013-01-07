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
        centerMapOn(address);
      });

      /* Filtering */
      
    }
  }

  function gmap_init(){
    // for content preview
    if (Drupal.settings.featureMap.lat && Drupal.settings.featureMap.lng) {
      set_preview_marker();
    }
    // for the general map
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
      url: Drupal.settings.featureMap.ajaxPath,
      data: {
        markers: markers
      },
      success : function(data) {
        var location = getDefaultLocation();
        var myOptions = {
          scrollwheel: false,
          zoom: 7,
          center: location, 
          mapTypeId: google.maps.MapTypeId.TERRAIN
        };
        map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);
        
        var marker = new google.maps.Marker({
              position: location,
              map: map,
              icon:  Drupal.settings.featureMap.modulePath + '/theme/blackmarker.png'
        });
        
        $.each(data, function(pos, data) {
          var marker = new google.maps.Marker({
              position: new google.maps.LatLng(data.lat,data.lng),
              icon:  Drupal.settings.featureMap.modulePath + '/theme/redmarker.png',
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
          
          $('#map_listing').append(data.full);
        });
      }
    });
  }

  function centerMapOn(address){
    if(typeof(address) == 'undefined') {
     var geocoder = new google.maps.Geocoder();
    geocoder.geocode( { 'address': address}, function(results, status) {
      if (status == google.maps.GeocoderStatus.OK) {
        $('#zipcodesearchform .location-error').remove();
        var location = results[0].geometry.location;     
      }
      else{
        $('#zipcodesearchform').append('<div class="location-error">' + Drupal.t('No results found') + '</div>');
      }
    });
  }

  map.setCenter(location);
  setLocationCookie(location);
  var marker = new google.maps.Marker({
    map: map,
    title: address,
    position: location,
    icon: Drupal.settings.featureMap.modulePath + "/theme/blackmarker.png"
  });
  }

  function getDefaultLocation() {
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
    setCookie('location', locationObject.toString(), 24);
  }
  
  function setCookie(c_name,value,exdays) {
    var exdate=new Date();
    exdate.setDate(exdate.getDate() + exdays);
    var c_value=escape(value) + ((exdays==null) ? "" : "; expires="+exdate.toUTCString());
    document.cookie=c_name + "=" + c_value;
  }

})(jQuery);


