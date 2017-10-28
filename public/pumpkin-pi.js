// these named CSS colors don't match the saturated led ones well
COLORMAP = {
    green:  '#00ff00',
    blue:   '#2040ff',
    purple: '#c000ff'
};

function mapColor(color) {
    return COLORMAP[color] || color;
}

function buildOffThing() {
    var thing = $('<span class="glyphicon glyphicon-ban-circle">');
    var sr_span = $('<span class="sr-only">').text('off');
    return $('<span class="color-menu-item">').append(thing).append(sr_span);
}

function buildColorSwatch(color) {
    var swatch = $('<span class="color-sel" style="background-color: ' + mapColor(color) + ';">');
    var sr_span = $('<span class="sr-only">').text(color);
    return $('<span class="color-menu-item">').append(swatch).append(sr_span);
}

function buildMenuItem(item) {
    if (item.value == 'off') {
        return buildOffThing();
    } else {
        return buildColorSwatch(item.value);
    }
}

function buildColorPicker(name, defaultColor) {
    var sel = 'select[name="' + name + '"]';
    $(sel).selectmenu({
        width: 75,
        select: function(event, ui) {
            var box = ui.item.element.parent().attr('name');
            localStorage[box] = ui.item.value;
        }
    });
    var menu = $(sel).selectmenu('instance');
    menu._renderItem = function(ul, item) {
        var li = $('<li>');
        buildMenuItem(item).appendTo(li);
        return li.appendTo(ul);
    }
    menu._renderButtonItem = function(item) {
        return buildMenuItem(item);
    }
    var color = localStorage[name] || defaultColor;
    $(sel).val(color);    
    $(sel).selectmenu('refresh');
}

function formSubmit(event) {
    event.preventDefault();
    var lcolor = $('#color_select').val();
    var rcolor = $('#color2_select').val();
    var url = $('#tts_radio').is(':checked') ? '/say' : '/play';
    var data = { text: $('#main-input').val() };
    if (rcolor == 'off') {
        data.channel = 'l';
        data.color = lcolor;
    } else if (lcolor == 'off') {
        data.channel = 'r';
        data.color = rcolor;
    } else {
        data.channel = 's';
        data.color = lcolor;
        data.color2 = rcolor;
    }
    $.ajax(url, { method: 'POST', data: data }).done(function(data) {
        console.log(data);
        $('#main-input').select().focus();
    });
}

$(document).ready(function() {
    buildColorPicker('color', 'cyan');
    buildColorPicker('color2', 'purple');
    $('#main-input').focus();
    $("#main-form").on("submit", formSubmit);
});