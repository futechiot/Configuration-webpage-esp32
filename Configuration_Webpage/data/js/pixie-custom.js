$(document).ready(function() {
    var options = [];
    $('.panel-title a').on('click', function() { var checkboxControl = $(this).find("input[type='checkbox']"); if (checkboxControl.prop('checked') == false) { checkboxControl.prop('checked', true); } else { checkboxControl.prop('checked', false); } });
   
       $('body').on('change', '.sevice-access', function(event) {
        if ($(this).find('option:selected').text() == "MQTT") {
            $(this).parents('#service').addClass('mqtt-box-active');
            $(this).parents('#service').removeClass('http-box-active');
            $(this).parents('#service').removeClass('tcp-box-active');
            $(this).parents('#service').removeClass('socket-box-active');
        }
    });

    $('body').on('change', '.sevice-access', function(event) {
        if ($(this).find('option:selected').text() == "HTTP") {
            $(this).parents('#service').addClass('http-box-active');
            $(this).parents('#service').removeClass('mqtt-box-active');
            $(this).parents('#service').removeClass('tcp-box-active');
            $(this).parents('#service').removeClass('socket-box-active');
        }
    });

    $('body').on('change', '.sevice-access', function(event) {
        if ($(this).find('option:selected').text() == "TCP") {
            $(this).parents('#service').addClass('tcp-box-active');
            $(this).parents('#service').removeClass('mqtt-box-active');
            $(this).parents('#service').removeClass('http-box-active');
            $(this).parents('#service').removeClass('socket-box-active');
        }
    });

    $('body').on('change', '.sevice-access', function(event) {
        if ($(this).find('option:selected').text() == "Socket.io") {
            $(this).parents('#service').addClass('socket-box-active');
            $(this).parents('#service').removeClass('mqtt-box-active');
            $(this).parents('#service').removeClass('http-box-active');
            $(this).parents('#service').removeClass('tcp-box-active');
        }
    });



    

    $(document).on('change', '.mdb-select', function(event) {
        if ($(this).find('option:selected').text() == "static") {
            $(this).parents('#network').addClass('static-box-active');
        } else {
            $(this).parents('#network').removeClass('static-box-active');
        }
    });
    $(document).on('change', '.mdb-select-repeater', function(event) {
        if ($(this).find('option:selected').text() == "static") {
            $(this).parents('#network').addClass('inner-static-box-active');
        } else {
            $(this).parents('#network').removeClass('inner-static-box-active');
        }
    });

    $('body').on('click', '.dhcp-box', function(event) { $(this).parents('#network').removeClass('static-box-active'); });
    $('body').on('click', '.inner-static-box', function(event) { $(this).parents('#network').addClass('inner-static-box-active'); });
    $('body').on('click', '.inner-dhcp-box', function(event) { $(this).parents('#network').removeClass('inner-static-box-active'); });
    $('body').on('change', '.ble-data-format1,.ble-data-format2,.ble-data-format3,.ble-data-format4,.upgeade-tpe-box ', function(event) { });//bleDataFormatFilter(); 
    //bleDataFormatFilter();
});

/*function bleDataFormatFilter() {
    var bleDataFormat1 = $('.ble-data-format1 option:selected');
    var bleDataFormat2 = $('.ble-data-format2 option:selected');
    if (bleDataFormat1.text() == "Json" && bleDataFormat2.text() == "Short") {
        $('.json-short-box').css('display', 'block');
        $('.binary-box,.json-long-box').css('display', 'none');
    } else if (bleDataFormat1.text() == "Json" && bleDataFormat2.text() == "Long") {
        $('.json-long-box').css('display', 'block');
        $('.binary-box,.json-short-box').css('display', 'none');
    } else if (bleDataFormat1.text() == "Binary" && (bleDataFormat2.text() == "Long" || bleDataFormat2.text() == "Short")) {
        $('.binary-box').css('display', 'block');
        $('.json-short-box,.json-long-box').css('display', 'none');
    }
    var bleDataFormat3 = $('.ble-data-format3 option:selected');
    var bleDataFormat4 = $('.ble-data-format4 option:selected');
    if (bleDataFormat3.text() == "Json" && bleDataFormat4.text() == "Short") {
        $('.json-short-box1').css('display', 'block');
        $('.binary-box1,.json-long-box1').css('display', 'none');
    } else if (bleDataFormat3.text() == "Json" && bleDataFormat4.text() == "Long") {
        $('.json-long-box1').css('display', 'block');
        $('.binary-box1,.json-short-box1').css('display', 'none');
    } else if (bleDataFormat3.text() == "Binary" && (bleDataFormat4.text() == "Long" || bleDataFormat4.text() == "Short")) {
        $('.binary-box1').css('display', 'block');
        $('.json-short-box1,.json-long-box1').css('display', 'none');
    }
    var bleDataFormat5 = $('.upgeade-tpe-box option:selected');
    if (bleDataFormat5.text() == "USB Upgrade") {
        $('.usb-upgeade-box').css('display', 'block');
        $('.ota-upgrade-box,.customization-box').css('display', 'none');
    } else if (bleDataFormat5.text() == "OTA Upgrade") {
        $('.ota-upgrade-box').css('display', 'block');
        $('.usb-upgeade-box,.customization-box').css('display', 'none');
    } else if (bleDataFormat5.text() == "Customization Upgrade") {
        $('.customization-box').css('display', 'block');
        $('.usb-upgeade-box,.ota-upgrade-box').css('display', 'none');
    }
}*/