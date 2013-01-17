(function($) {
  Drupal.behaviors.GmapLocator = {
    // var definitions
    map: undefined,
    markers: [],

    attach: function(context) {
      /*  Preview image */
      if (Drupal.settings.featureMap.lat && Drupal.settings.featureMap.lng) {
        if($('#gmap').length){
          set_preview_marker();
        }
        
      }
      /*  Basic Map Page */
      else{
        if($('#map_canvas').length){
        // Default build
          buildMapAround('default');
        }
        // use location search box
        $('#zipcodesearchform .location-submit').bind('click', function(e){
          e.preventDefault();
          var address = $('#zipcodesearchform #location').val();
          buildMapAround(address);

        });

      }
    }
  }


/* FUNCTIONS */

  function set_preview_marker() {
    var myLatlng = new google.maps.LatLng(Drupal.settings.featureMap.lat,Drupal.settings.featureMap.lng);
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
  
  function buildMapAround(address){
    if(address == 'default') {
      var location = getDefaultLocation();
      centerMapOn(location);
    }
    else {
      var geocoder = new google.maps.Geocoder();
      geocoder.geocode( {'address': address}, function(results, status) {
        if (status == google.maps.GeocoderStatus.OK) {
          $('#zipcodesearchform .location-error').remove();
          var location = results[0].geometry.location;
          centerMapOn(location);
          setLocationCookie(location);
        }
        else{
          $('#zipcodesearchform').append('<div class="location-error">' + Drupal.t('No results found') + '</div>');
        }
      });
    }
  }


  function load_markers(map) {

    // don't allow simultaneous loads
    try{xhr.abort();}
    catch(err){}
    
    var markers = 'initial'; // devQ: filter on markers in ajax or with js???

    xhr = $.ajax({
      type: 'POST',
      url: Drupal.settings.featureMap.ajaxPath,
      data: {
        markers: markers
      },
      success : function(data) {
        var location = getDefaultLocation();
        
        var marker = new google.maps.Marker({
              position: location,
              map: map,
              icon:  Drupal.settings.featureMap.modulePath + '/images/blackmarker.png'
        });
        
        $.each(data, function(pos, data) {
          var marker = new google.maps.Marker({
              position: new google.maps.LatLng(data.lat,data.lng),
              icon:  Drupal.settings.featureMap.modulePath + '/images/marker.png',
              html: data.teaser,
              month: data.month,
              id: pos,
              map: map
            });
            
            // To add the marker to the map, call setMap();
          infoBubble = new InfoBubble ({
            shadowStyle: 1,
            padding: 2,
            backgroundColor: 'rgb(57,57,57)',
            borderRadius: 0,
            arrowSize: 5,
            borderWidth: 1,
            borderColor: '#000',
            disableAutoPan: true,
            hideCloseButton: true,
            arrowPosition: 0,
            backgroundClassName: 'infoBubble',
            arrowStyle: 2,
            disableAnimation:true
          });
          
          $('#map_listing').append('<li class="marker-' + marker.id + '">' + data.full + '</li>');         
          
          google.maps.event.addListener(marker, 'click', function() {
            infoBubble.setContent('<div class="infotext">' + this.html + '</div>');
            infoBubble.open(map, this);
            $('#map_listing li, #map_timeline li').removeClass('active');
            $('.marker-' + marker.id).addClass('active');
            $('.month-' + marker.month).parent('li').addClass('active');
          });
          
        });
      }
    });
  }

  function centerMapOn(location) {
    var mapOptions = {
      scrollwheel: false,
      zoom: 7,
      center: location,
      mapTypeId: google.maps.MapTypeId.TERRAIN
    };
    map = new google.maps.Map(document.getElementById("map_canvas"), mapOptions);
    map.setCenter(location);
    var marker = new google.maps.Marker({
      map: map,
      position: location,
      icon: Drupal.settings.featureMap.modulePath + "/images/blackmarker.png"
    });
    load_markers(map);
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
    setCookie('location', locationObject.toString());
  }
  
  function setCookie(c_name,value,exdays) {
    var exdate=new Date();
    exdate.setDate(exdate.getDate() + exdays);
    var c_value=escape(value) + ((exdays==null) ? "" : "; expires="+exdate.toUTCString());
    document.cookie=c_name + "=" + c_value;
  }

})(jQuery);


