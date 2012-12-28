(function($) {

  Drupal.behaviors.featureMap = {
    attach: function(context) {
      console.log('test');
      gmap_init();
      
    }
  }

  function gmap_init(){
    alert('initialising');
  }

//
//
//
//  var brussels = new google.maps.LatLng(50.833, 4.333);
//  var initialLocation = get_default_location();
//
//  var browserSupportFlag = new Boolean();
//  var geocoder;
//  var map;
//  var markerClusterAgents;
//  var markers = [];
//  var infoWindow;
//  var myLocationMarker=false;
//  var asciiA="A".charCodeAt(0);
//  var savedzipcode=false;
//
//  var activeBranch=null;
//
//  Drupal.behaviors.branchLocator = {
//    attach: function(context) {
//      // Initialize Google Maps
//      initializeGoogleMaps();
//      $("#locate").click(function(e) {
//        e.preventDefault();
//        findMyAddress($("#location").val());
//      });
//      $('#categories input:checkbox').click(function() {
//        if(myLocationMarker)loadBranches(true,savedzipcode, 0);
//      });
//      $("#resultlist li").live("mouseover",function(){
//          $(this).hoverIntent( //only open the branch if the user intended to
//             function(){/*mouseover logic*/
//                var agent_id=$(this).attr("id").substr(9);
//                if (agent_id!=activeBranch) activeBranch = agent_id;
//                else return; // prevent redrawing when the selected branch is the same
//                var marker = getMarker(agent_id);
//                if(marker) {
//                    //map.panTo(marker.position);
//                    google.maps.event.trigger(marker, 'click');
//                }
//          }, function() {/*mouse out logic*/});
//          $(this).trigger('mouseover');
//
//      });
//      $("#category_credits_all").click(function() {
//        $(this).parents('li').toggleClass('collapse');
//        var checkstate = $(this).attr("checked");
//        if (checkstate) {
//          $('.agent-selection-level-two').removeClass('hidden');
//        }
//        else {
//          $('.agent-selection-level-two').addClass('hidden');
//        }
//        $('.agent-selection-level-two input:checkbox').each(function() {
//          this.checked = checkstate;
//        });
//      });
//      $('.agent-selection-level-two input:checkbox').click(function() {
//        if (!$('.agent-selection-level-two input:checked').length) {
//           $("#category_credits_all").attr('checked', false);
//        }else {
//          $("#category_credits_all").attr('checked', true);
//        }
//      });
//        $('#agentsort').click(function() {
//            var clonedmarkers = markers.slice(0);
//            if($(this).attr('value')== "name"){
//                clonedmarkers=sortByName(clonedmarkers);
//            } else if($(this).attr('value') =="distance"){
//                clonedmarkers=sortByDistance(clonedmarkers);
//            }
//            $("#agentlist").html("");
//            for(var index in clonedmarkers){
//                var marker=clonedmarkers[index];
//                var listItem=$(marker.agent.list_item_html);
//                if(listItem.hasClass('sort')){
//                  $(".markericon",listItem).append($("<img>").attr("src",marker.icon));
//                  $(".distance",listItem).text(marker.distance);
//                  $("#agentlist").append(listItem);
//                }
//            }
//        });
//    }
//  };
//
//    function initialSortByDistance() {
//      var clonedmarkers = markers.slice(0);
//      clonedmarkers=sortByDistance(clonedmarkers);
//      if($('#agentlist li').not('.sort').length) {
//      $("#agentlist li").not('.sort').remove();
//        for(var index in clonedmarkers){
//            var marker=clonedmarkers[index];
//            var listItem=$(marker.agent.list_item_html);
//            if(!listItem.hasClass('sort')){
//              $(".markericon",listItem).append($("<img>").attr("src",marker.icon));
//              $(".distance",listItem).text(marker.distance);
//              $("#agentlist").append(listItem);
//            }
//        }
//      }
//    }
//
//    function sortByDistance(clonedmarkers){
//        clonedmarkers.sort(function(markera,markerb){
//            var nameA=markera.distancenumber, nameB=markerb.distancenumber;
//            if (nameA < nameB) //sort string ascending
//                return -1;
//            if (nameA > nameB)
//                return 1;
//            return 0 ;//default return value (no sorting)
//        });
//        return clonedmarkers;
//    }
//
//    function sortByName(clonedmarkers){
//        clonedmarkers.sort(function(markera,markerb){
//            var nameA=markera.agent.name.toLowerCase(), nameB=markerb.agent.name.toLowerCase();
//            if (nameA < nameB) //sort string ascending
//                return -1;
//            if (nameA > nameB)
//                return 1;
//            return 0 ;//default return value (no sorting)
//        });
//        return clonedmarkers;
//    }
//
//  function get_initial_location(checkHashParams) {
//    // Try W3C Geolocation (Preferred)
//    var hashlocation = false;
//    if(checkHashParams){
//      var hashparams = getHashParams();
//      if(hashparams){
//        // set refereed options based on referring page
//          set_referred_options(hashparams.source);
//        // set location when redirected
//        if(hashparams.q){
//          $("#location").val(hashparams.q);
//          hashlocation = findMyAddress(hashparams.q);
//        }
//      }
//    }
//    if(hashlocation){
//      // don't do anything. hashlocation is asynch so we'll handle it from there
//      //initialLocation = hashlocation;
//    } else if (navigator.geolocation) {
//      browserSupportFlag = true;
//      navigator.geolocation.getCurrentPosition(function(position) {
//        initialLocation = new google.maps.LatLng(position.coords.latitude, position.coords.longitude);
//        $("#location").val(get_displayname_from_postion(position));
//        findMyAddress($("#location").val());
//        map.setCenter(initialLocation);
//      }, function() {
//        initialLocation = get_default_location(browserSupportFlag);
//      });
//    // Try Google Gears Geolocation
//    } else if (google.gears) {
//      browserSupportFlag = true;
//      var geo = google.gears.factory.create('beta.geolocation');
//      geo.getCurrentPosition(function(position) {
//        initialLocation = new google.maps.LatLng(position.latitude, position.longitude);
//        $("#location").val(get_displayname_from_postion(position));
//        findMyAddress($("#location").val());
//        map.setCenter(initialLocation);
//      }, function() {
//        initialLocation = get_default_location(browserSupportFlag);
//      });
//    // Browser doesn't support Geolocation
//    } else {
//      browserSupportFlag = false;
//      initialLocation = get_default_location(browserSupportFlag);
//
//    }
//    return initialLocation;
//  }
//
//  function set_referred_options(source) {
//    switch(source) {
//      case 'banking':
//        $("#category_banking_products").attr('checked', true);
//        break;
//      case 'invest':
//        $("#category_saving_products").attr('checked', true);
//        break;
//      case 'save' :
//        $("#category_saving_products").attr('checked', true);
//        break;
//      case 'loan' :
//        $("#category_credits_all").attr('checked', true);
//        break;
//      default:
//        $("#category_banking_products").attr('checked', true);
//        $("#category_saving_products").attr('checked', true);
//        $("#category_credits_all").attr('checked', true);
//        break;
//    }
//  }
//
//  function get_displayname_from_postion(position){
//    var displayname="";
//    if(position.address) {
//      if(position.address.street!=null) displayname+=position.address.street;
//      if(displayname!="") displayname+=' ';
//      if(position.address.streetNumber!=null) displayname+=position.address.Number;
//      if(displayname!="") displayname+=', ';
//      if(position.address.city!=null) displayname+=position.address.city;
//      // Hiding the country to enable displaying the branches by cityname first in initial search.
////      if(displayname!="") displayname+=', ';
////      if(position.address.country!=null) displayname+=position.address.country;
//    }
//
//    return displayname;
//  }
//
//  function initializeGoogleMaps() {
//    geocoder = new google.maps.Geocoder();
//    var myOptions = {
//      scrollwheel: false,
//      zoom: 7,
//      center: brussels,
//      mapTypeId: google.maps.MapTypeId.ROADMAP
//    };
//    var map = new google.maps.Map(document.getElementById("map_canvas"),
//      myOptions);
//
//
////    infoWindow.addTab(gmapsettings.tab_info_title, "");
//    var markerStyles = [{url: '/sites/all/themes/benny/images/map/group-empty.png',height: 33, width: 33,opt_textColor: '#000000'}];
//    var mcOptions = {
//      gridSize: 10,
//      maxZoom: 15,
//      averageCenter:true,
//      batchSizeIE:50,
//      styles: markerStyles
//    };
//    markerClusterAgents = new MarkerClusterer(map, [], mcOptions);
//    initialLocation = get_initial_location(true);
//    if(initialLocation) map.setCenter(initialLocation);
//
//    google.maps.event.addListener(markerClusterAgents, "click", function (c) {
////      log("click: ");
////      log("&mdash;Center of cluster: " + c.getCenter());
////      log("&mdash;Number of managed markers in cluster: " + c.getSize());
//      var m = c.getMarkers();
//      var p = [];
//      for (var i = 0; i < m.length; i++) {
//        p.push(m[i].getPosition());
//      }
////      log("&mdash;Locations of managed markers: " + p.join(", "));
//
//    });
//  }
//
//  function openInfoWindow(marker) {
//      infoWindow.updateTab(0, gmapsettings.tab_info_title, marker.agent.tab_info_html);
//      infoWindow.updateTab(1, gmapsettings.tab_hours_title, marker.agent.tab_hours_html);
//      infoWindow.updateTab(2, gmapsettings.tab_services_title, marker.agent.tab_services_html);
//      if (!infoWindow.isOpen()) {
//        infoWindow.open(map, marker);
//      }
//  }
//
//  function setCookie(c_name, value, exdays) {
//    var exdate = new Date();
//    exdate.setDate(exdate.getDate() + exdays);
//    var c_value = escape(value) + ((exdays == null) ? "" : "; expires=" + exdate.toUTCString());
//    document.cookie = c_name + "=" + c_value;
//  }
//  function getCookie(c_name) {
//    var i,x,y,ARRcookies = document.cookie.split(";");
//    for (i = 0; i < ARRcookies.length; i++) {
//      x = ARRcookies[i].substr(0, ARRcookies[i].indexOf("="));
//      y = ARRcookies[i].substr(ARRcookies[i].indexOf("=") + 1);
//      x = x.replace(/^\s+|\s+$/g, "");
//      if (x == c_name) {
//        return unescape(y);
//      }
//    }
//    return false;
//  }
//  function log(h) {
//    document.getElementById("log").innerHTML += h + "<br />";
//  }
//
//  function get_default_location(errorFlag) {
//    var cookieLocation = getCookie("mylastlocation");
//    if (errorFlag) {
//    initialLocation = brussels;
//    }
//    return (!cookieLocation) ? brussels : cookieLocation;
//  }
//
//  function findMyAddress(address) {
//    if(address!=""){
//      geocoder.geocode({
//        'address': address,
//        'region':'be'
//      }, function(results, status) {
//        if (status == google.maps.GeocoderStatus.OK) {
//          map.setCenter(results[0].geometry.location);
//          if (myLocationMarker) myLocationMarker.setMap(null);
//          myLocationMarker = new google.maps.Marker({
//            map: map,
//            position: results[0].geometry.location,
//            icon: "/sites/all/themes/benny/images/map/rb_here_"+language+'.png'
//          });
//          myLocationMarker.setMap(map);
//          map.setZoom(gmapsettings.default_zoom);
//          savedzipcode=getZipcodeFromGoogleLocation(results);
//            if(savedzipcode==false){
//                savedzipcode=getZipcodeFromAddress(address);
//            }
//          loadBranches(true,savedzipcode, 0);
//          return true;
//        } else {
//          return false;
//          alert("Geocode was not successful for the following reason: " + status);
//        }
//      });
//      return true;
//    } else {
//    return false;
//    }
//  }
//
//
//  function getZipcodeFromGoogleLocation(googleresults){
//      var zipcode = false;
//      var i,j,k;
//      for(i=0;i<googleresults.length;i++){
//          var components = googleresults[i].address_components;
//          for (j = 0; j < components.length; j++) {
//              var types = components[j].types;
//              for (k = 0; k < types.length; k++) {
//                  if (types[k] === 'postal_code'){
//                      zipcode = components[j].long_name;
////                      console.log("FOUND type:"+",i:"+i+",j:"+j+",k:"+k+":"+types[k] + ",long_name"+components[j].long_name);
//                  } else {
////                      console.log("type:"+",i:"+i+",j:"+j+",k:"+k+":"+types[k] + ",long_name"+components[j].long_name);
//                  }
//              }
//          }
//          if(zipcode) return zipcode;
//      }
////      console.log("zipcode:"+zipcode+",i:"+i+",j:"+j+",k:"+k);
//      return zipcode;
//  }
//
//    function getZipcodeFromAddress(address){
//        var pattern1 = /^([0-9]{4})( |,).*/i;
//        var pattern2 = /.*([0-9]{4})( |,).*/i;
//        var matches = pattern1.exec(address);
//        if(matches){
////            console.log("match found for pattern 1:"+matches[1]);
//            return matches[1];
//        } else {
//            matches = pattern2.exec(address);
//            if(matches){
////                console.log("match found for pattern 2:"+matches[1]);
//                return matches[1];
//            }
//        }
////        console.log("no match found .");
//        return address;
//    }
//  function getMarker(id){
//    for(i=0;i<markers.length;i++){
//      var marker=markers[i];
//      if(marker.agent && marker.agent.id==id){
//        return marker;
//      }
//    }
//
//    return false;
//  }
//
//  var activeConnections = 0;
//
//  function loadBranches(isInitialSearch,zipcode, totalBranches) {
//    $('#content').css('cursor', 'wait');
//    var bounds = map.getBounds();
//    // Then the points
//    var swPoint = bounds.getSouthWest();
//    var nePoint = bounds.getNorthEast();
//    var locatordata = {
//      nelat:nePoint.lat(),
//      nelng:nePoint.lng(),
//      swlat:swPoint.lat(),
//      swlng:swPoint.lng()
//    };
//    $('#categories input:checkbox').each(function() {
//      if (this.checked) {
//        locatordata[$(this).val()] = 1;
//      }
//    });
//    locatordata["totalbranches"] = totalBranches;
//    locatordata["language"] = language;
//    if(isInitialSearch){
//      locatordata["postcode"] = zipcode;
//      locatordata["initial"] = true;
//    }
//
//    try{xhr.abort();}
//    catch(err){}
//
//    xhr = $.post(gmapsettings.agent_json_url, locatordata, function(data, textStatus) {
//      if(isInitialSearch){
//        markerClusterAgents.clearMarkers();
//        markers = [];
//        $("#agentlist").html('');
//      }
//      $.each(data.agents, function(pos, agent) {
//        if(isInitialSearch || !getMarker(agent.id)){
//          var markerLetter=String.fromCharCode(asciiA+markers.length);
//          var markerIcon=agent.marker_icon.replace(".png","_"+markerLetter+".png").toLowerCase();
//          var listItem=$(agent.list_item_html);
//          $(".markericon",listItem).append($("<img>").attr("src",markerIcon));
//          $("#agentlist").append(listItem);
//          var marker = new google.maps.Marker({
//            agent: agent,
//            distance:'',
//            distancenumber:9999999,
//            map: map,
//            position: new google.maps.LatLng(agent.lat, agent.lng),
//            icon: markerIcon
//          });
//          markers.push(marker);
//          google.maps.event.addListener(marker, 'click', function() {
//            infoWindow.close();
//            openInfoWindow(marker);
//            makeListItemActive(marker);
//            infoWindow.panToView();
//          });
//          markerClusterAgents.addMarker(marker);
////          if(markerClusterAgents.getTotalMarkers()>=gmapsettings.max_result_count){
////            return false; // break the loop
////          }
//        }
//
//      });
//      $("#agentresultcount").text(markerClusterAgents.getTotalMarkers());
//      if(isInitialSearch){map.setZoom(gmapsettings.default_zoom);}
//      if(markerClusterAgents.getTotalMarkers()<gmapsettings.min_result_count){
////        if(map.getZoom()>gmapsettings.max_zoom) map.setZoom(map.getZoom()-1);
//        map.setZoom(map.getZoom()-1);
//        if(map.getZoom()>gmapsettings.min_zoom_for_min_result) {
//          loadBranches(false,null,markerClusterAgents.getTotalMarkers());
//        }
//      }
//      calculatedistances();
//      setTimeout(function(){initialSortByDistance()}, 1000);
//      $('#content').css('cursor', 'default');
//    }, "json");
//
//  }
//
//  function makeListItemActive(marker){
//    $('#agentlist .active').removeClass("active");
//    $('#li-agent-'+marker.agent.id).addClass("active");
//  }
//
//  function calculatedistances() {
//    var origin=myLocationMarker.position;
//    var destinations=[];
//    for(var i=0;i<markers.length;i++){
//        destinations.push(markers[i].position)
//    }
//    var service = new google.maps.DistanceMatrixService();
//    service.getDistanceMatrix(
//    {
//      origins: [origin],
//      destinations: destinations,
//      travelMode: google.maps.TravelMode.DRIVING,
//      avoidHighways: false,
//      avoidTolls: false
//    }, distancecallback);
//  }
//
//
//  function distancecallback(response, status) {
//    // See Parsing the Results for
//    // the basics of a callback function.
//    if (status == google.maps.DistanceMatrixStatus.OK) {
//      var origins = response.originAddresses;
//      var destinations = response.destinationAddresses;
//
//      for (var i = 0; i < origins.length; i++) {
//        var results = response.rows[i].elements;
//        for (var j = 0; j < results.length; j++) {
//          var element = results[j];
//          var marker = markers[j];
//          if(element.distance && marker){
//            var distance = element.distance.text;
//            var duration = element.duration.text;
//            var from = origins[i];
//            var to = destinations[j];
//            var li = $("#li-agent-"+marker.agent.id);
//            $(".distance",li).text(distance + " (" + duration + ")");
//            marker.distance=distance + " (" + duration + ")";
//            var distanceregex = marker.distance.match(/[\d\.,]+/g);
//              if(distanceregex.length>0){
//                marker.distancenumber=parseFloat(distanceregex[0].replace(",","."));
//              } else {
//                marker.distancenumber=999999999;
//              }
//
//
//            marker.agent.tab_info_html=marker.agent.tab_info_html.replace('<span class="distance"></span>','<span class="distance">'+marker.distance+'</span>');
//          } else if(marker){
//              marker.distance='';
//              marker.distancenumber=999999999;
//          }
//        }
//      }
//    }
//  }
//
//  function infowin(bounds, html) {
//    this.prototype = new google.maps.OverlayView();
//    this.prototype.bounds_ = bounds;
//    this.prototype.html_ = html;
//    this.prototype.map_ = map;
//    this.prototype.div_ = null;
//    this.prototype.setMap(map);
//    this.prototype.onAdd = function() {
//      // Note: an overlay’s receipt of onAdd() indicates that
//      // the map’s panes are now available for attaching
//      // the overlay to the map via the DOM.
//      // Create the DIV and set some basic attributes.
//      var div = document.createElement('DIV');
//      div.style.borderStyle = "none";
//      div.style.borderWidth = "0px";
//      div.style.position = "absolute";
//      //div.style.width = "150px”;
//      //div.style.height = "80px”;
//      div.className = "customInfo";
//      div.innerHTML = this.html_;
//      // Set the overlay's div_ property to this DIV
//      this.div_ = div;
//      // We add an overlay to a map via one of the map's panes.
//      // We'll add this overlay to the overlayImage pane.
//      var panes = this.getPanes();
//      panes.overlayImage.appendChild(div);
//    }
//    this.prototype.draw = function() {
//      // Size and position the overlay. We use a southwest and northeast
//      // position of the overlay to peg it to the correct position and size.
//      // We need to retrieve the projection from this overlay to do this.
//      var overlayProjection = this.getProjection();
//      // Retrieve the southwest and northeast coordinates of this overlay
//      // in latlngs and convert them to pixels coordinates.
//      // We'll use these coordinates to resize the DIV.
//      var sw = overlayProjection.fromLatLngToDivPixel(this.bounds_.getSouthWest());
//      var ne = overlayProjection.fromLatLngToDivPixel(this.bounds_.getNorthEast());
//      // Resize the image's DIV to fit the indicated dimensions.
//      var div = this.div_;
//      div.style.left = sw.x + 'px';
//      div.style.top = ne.y + 'px';
//    //div.style.width = (ne.x – sw.x) + ‘px';
//    //div.style.height = (sw.y – ne.y) + ‘px';
//    }
//    this.prototype.onRemove = function() {
//      this.div_.parentNode.removeChild(this.div_);
//      this.div_ = null;
//    }
//
//  }
//
//
//  function getHashParams() {
//
//    var hashParams = {};
//    var e,
//    a = /\+/g,  // Regex for replacing addition symbol with a space
//    r = /([^&;=]+)=?([^&;]*)/g,
//    d = function (s) {
//      return decodeURIComponent(s.replace(a, " "));
//    },
//    q = window.location.hash.substring(1);
//
//    while (e = r.exec(q))
//      hashParams[d(e[1])] = d(e[2]);
//
//    return hashParams;
//  }
//
//  function parseAddress(term) {
//    geocoder.geocode({
//      'address': term,
//      'region':'be'
//    }, function(results, status) {
//      if (status == google.maps.GeocoderStatus.OK) {
//        response($.map(results, function(item) {
//          return {
//            label:  item.formatted_address,
//            value: item.formatted_address,
//            latitude: item.geometry.location.lat(),
//            longitude: item.geometry.location.lng()
//          }
//        }));
//      } else {
//    //alert("Geocode was not successful for the following reason: " + status);
//    }
//    });
//  /**
//    geocoder.geocode( {'address': request.term }, function(results, status) {
//          response($.map(results, function(item) {
//            return {
//              label:  item.formatted_address,
//              value: item.formatted_address,
//              latitude: item.geometry.location.lat(),
//              longitude: item.geometry.location.lng()
//            }
//          }));
//            return $.map(data, function(item) {
//            return {
//              label:  item.formatted_address,
//              value: item.formatted_address,
//              latitude: item.geometry.location.lat(),
//              longitude: item.geometry.location.lng()
//            }
//          });
//     **/
//  }
//
//  function formatItem(data) {
//    return data.label;
//  }
//
//  function formatResult(data) {
//    return data.label;
//  }
//
//  function loadAddress(term){
////    console.log("LoadAddress called with therm "+term);
//    return [];
//  }
//
//})(jQuery);
//
