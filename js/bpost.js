(function($) {
  Drupal.behaviors.bpost = {
    attach: function(context) {  
      if (Drupal.settings.bpost) {  
          $('.bpost_autocomplete').autocomplete(Drupal.settings.bpost.process_path, {
            cacheLength:10,
            max: 20,
            delay: 10,
            minChars: 1,
            matchContains: false,
            scroll:false,
            parse: this.parseJson,
            dataType: 'json',
            extraParams:{
              lang:Drupal.settings.bpost.language
              },
            formatItem: this.formatItem,
            formatResult: this.formatResult
          }).result(function(event, item) {
            });
          $('.trigger-lookup.form-submit').bind('click', function(e) {
            e.preventDefault();
            var action_url = $(this).parents('form').attr('action');
            var input = $('.bpost_autocomplete').val();
//            window.location = window.location.protocol + '//' + window.location.hostname + '/' + Drupal.settings.bpost.language + '/map#q=' + input;
            window.location = window.location.protocol + '//' + window.location.hostname + '/map#q=' + input;
          });
      }
    },
    parseJson: function(data){
      var zipcodes=[];
      for (var i = 0; i < data.length; i++) {     
        var displayName =  this.formatItem(data[i]);        
        zipcodes[i] = {
          value: displayName,      
          result: displayName,
          data: data[i]
        };     
      }
      return zipcodes;
    },
    formatItem: function(data, pos, max, value, term) {
      var displayName = (Drupal.settings.bpost.language=="fr")?data.name_fr:data.name_nl;
      // check: if empty, use the other language
      if(displayName=="") displayName = (Drupal.settings.bpost.language=="fr")?data.name_nl:data.name_fr;
      var zipcode=data.zipcode;
      return zipcode + ', ' + displayName;
    //return zipcode + ' - ' + displayName;
    },
    formatResult: function(data) {
      var displayName = (Drupal.settings.bpost.language=="fr")?data.name_fr:data.name_nl;
      // check: if empty, use the other language
      if(displayName=="") displayName = (Drupal.settings.bpost.language=="fr")?data.name_nl:data.name_fr;
      var zipcode=data.zipcode;
      return zipcode + ', ' + displayName;
    }  
  } 
})(jQuery);
