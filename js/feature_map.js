(function($){
  
  var language = 'fr'; // Default to french;
  
  // This is no best practice. But because of google maps 
  var agent_path = '/sites/all/modules/features/feature_branches/callbacks/feature_branches.my_agent.php';
  var header_path = '/sites/all/modules/features/feature_branches/callbacks/feature_branches.agent_header.php';
  var current_element;
  
  Drupal.behaviors.featureMap = {
    attach: function(context) { 
      
      language = $('html').attr('lang'); 
      
      var agent = getCookie('agent');
      
      if(agent && !$('body').hasClass('page-agents') && !$('body').hasClass('node-type-branch')) {
        $('.btn-appointment').click(function(event){
          event.preventDefault();
          $.colorbox({
          href:'/agents/contact-form/' + agent + '/' + language,
          width:'560px',
          height:'545px',
          iframe:true
          });
        })
      }
      
      // add the current page to the agent locator link
      if($('.menu-level-1 a.active-trail').length > 0){
        if($('.menu-level-1 a.active-trail').attr('id')) {
          set_locator_callback();
        }
      }
      
      if ($('.my-agent').length) {
        this.branch_check_agent();
      }
      if($('.trigger-fill').length) {
        $('.trigger-fill').click(function(){
          $.post("../ajax/check-agent", {
          op : "check-agent"
          }, agent_handle_result );
        });
      }   

      if($('.bpost_autocomplete').length) {
        this.clear_formfield();
      }            
      if($('#agent_gmap').length) {
        this.branch_gmap_init();
      }
      console.log($().jquery);
//      if($('#slider-range').length){
//        $( "#slider-range" ).slider({
//          range: "min",
//          value: 250,
//          min: 0,
//          max: 1000,
//          slide: function( event, ui ) {
//            $( "#amount" ).val( ui.value + " km");
//          }
//        });
//        $( "#amount" ).val( $( "#slider-range" ).slider( "value" ) + " km" );
//      }
      

    },
    
    clear_formfield: function() {
      $('.bpost_autocomplete').click(function(){
       this.value = '';
      })
    },
    branch_gmap_init: function() {
      if (Drupal.settings.agent_gmap) {
  
        var myLatlng = new google.maps.LatLng(Drupal.settings.agent_gmap.lat,Drupal.settings.agent_gmap.lng);
        var myOptions = {
          zoom: 10,
          center: myLatlng,
          mapTypeId: google.maps.MapTypeId.ROADMAP,
          disableDefaultUI: true
        }
        var map = new google.maps.Map(document.getElementById("agent_gmap"), myOptions);

        //      var infowindow = new google.maps.InfoWindow({
        //        content: Drupal.settings.agent_gmap.content,
        //        maxWidth: 250
        //      });

        var marker = new google.maps.Marker({
          position: myLatlng, 
          map: map,
          title:Drupal.settings.agent_gmap.title
        });
      
      //      infowindow.open(map, marker);      
      }
    },
    branch_check_agent: function() {      
      $.post(agent_path, {
        op : "check-agent",
        aid : Drupal.settings.feature_branches.aid
      }, this.agent_handle_result, 'json');
    },
    branch_update_agent: function(e, nid) {
      current_element = e;
      $.post(agent_path, {
        op : "update-agent",
        aid : nid,
        language: language
      }, this.agent_handle_result, 'json');
      
      // resetting all other agents
      if(('#agentlist').length > 0) {
        $('#agentlist .my-agent').each(function(){
          if($(this).attr('nid') != nid){
            $(this).html('<a class="btn-link btn-agent agent-set" href="#"><span>' + Drupal.t('Set this agent as your agent') + '</span></a>');
          }
        });
      }
    },
    branch_update_header: function() {
      $.post(header_path, { 
        format : 'json',
        language: language
      }, this.header_handle_result, 'json');
    },
    header_handle_result: function(response) {
      $('#agent-header-placeholder').empty().html(response.data);
    },
    agent_handle_result: function(response) {
      if(response.status && response.data) {
        $(current_element).html('<a class="btn-link btn-my-agent agent-unset" href="#"><span>' + Drupal.t('This is your agent') + '</span></a>');
      }
      else {
        $(current_element).html('<a class="btn-link btn-agent agent-set" href="#"><span>' + Drupal.t('Set this agent as your agent') + '</span></a>');
      }
      Drupal.behaviors.featureBranches.branch_update_header(); 
    }, 
    trigger_overlay: function(element) {
     $.colorbox({
        href:$(element).attr('href'),
        width:'560px',
        height:'545px',
        iframe:true
      });
    }
  }
  
  /*
   * Function was replaced by submit callback  in nas/helpers/bpost.js
   */
  function set_locator_callback(){
    // Getting the active main menu item 
    // and adding it to the agent-locator link.
    // this will be used to set the active options on the agent locator.
     var source = $('.menu-level-1 a.active-trail').attr('id');
     source = source.replace("nav-", "");
     var locatorlink = $('.nav-locator').attr('href') + '#source=' + source;
     $('.nav-locator').attr('href', locatorlink);
     
     if($('.locator-block-source').length > 0 ) {
       $('.locator-block-source').attr('value', source);
     }
     
  }
function getCookie(c_name) {
      var i,x,y,ARRcookies=document.cookie.split(";");
      for (i=0;i<ARRcookies.length;i++)
      {
        x=ARRcookies[i].substr(0,ARRcookies[i].indexOf("="));
        y=ARRcookies[i].substr(ARRcookies[i].indexOf("=")+1);
        x=x.replace(/^\s+|\s+$/g,"");
        if (x==c_name)
          {
          return unescape(y);
          }
        }
      }


})(jQuery);
