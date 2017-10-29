// these named CSS colors don't match the saturated led ones well
COLORMAP = {
    green:  '#00ff00',
    blue:   '#2040ff',
    purple: '#c000ff'
};

MAX_HISTORY_SIZE = 25;
HISTORY_VERSION = "1";

// initial history items (these will be replaced as the user types messages)
DEFAULT_HISTORY = [
    { url: '/say', lcolor: 'cyan', rcolor: 'off', text: "Hello! I'm the pumpkin down here.", pinned: true },
    { url: '/say', lcolor: 'green', rcolor: 'off', text: "Happy halloween!", pinned: true },
    { url: '/say', lcolor: 'yellow', rcolor: 'off', text: "You're welcome! Don't eat it all at once.", pinned: true },
    { url: '/play', lcolor: 'off', rcolor: 'red', text: "evil_laugh.mp3", pinned: true },
    { url: '/play', lcolor: 'off', rcolor: 'green', text: "sb_duck.mp3", pinned: true }
];

function buildPlayButton(url) {
    var $icon = $('<span class="glyphicon glyphicon-' + ((url == '/say') ? 'comment' : 'volume-up') + '">');
    return $("<button class='btn btn-" + ((url == '/say') ? 'primary' : 'danger') + " play-button'>").append($icon);
}

function buildPinButton(pinned) {
    return $("<button class='btn btn-" + (pinned ? 'primary' : 'default') + " pin-button'>").append($("<span class='glyphicon glyphicon-pushpin'>"));    
}

function createHistoryElement(item) {
    var $el = $('<div class="hist-elem">');
    if (item.pinned) {
        $el.addClass('pinned');
    }
    $el.append(buildPlayButton(item.url));
    $el.append(buildColorItem(item.lcolor, 'hist-swatch'));
    $el.append(buildColorItem(item.rcolor, 'hist-swatch'));
    $el.append($('<span class="text">').text(item.text).addClass((item.url == '/play') ? 'sound-effect' : 'to-speech'));
    $el.append(buildPinButton(item.pinned));
    $el.data('text', item.text);
    $el.data('url', item.url);
    $el.data('lcolor', item.lcolor);
    $el.data('rcolor', item.rcolor);
    $el.data('pinned', item.pinned);
    return $el;
}

function loadHistory() {
    var history = (localStorage.history_version == HISTORY_VERSION) ? JSON.parse(localStorage.history) : DEFAULT_HISTORY;
    if (history.length == 0) {
        history = DEFAULT_HISTORY;
    }
    var $historyContainer = $("#history");
    history.forEach(function(item) {
        $historyContainer.append(createHistoryElement(item));
    });
}

function parseHistoryElement(el) {
    var $el = $(el);
    var item = { url: $el.data('url'), lcolor: $el.data('lcolor'), rcolor: $el.data('rcolor'), text: $el.data('text'), pinned: $el.data('pinned') };
    return item;
}

function saveHistory() {
    var history = [];
    $("#history .hist-elem.pinned").each(function(index, el) {
        history.push(parseHistoryElement(el));
    });
    localStorage.history = JSON.stringify(history);
    localStorage.history_version = HISTORY_VERSION;
}

function findHistoryElement(url, text) {
    return $("#history .hist-elem").filter(function(index, el) {
        var $el = $(el);
        return $el.data('url') == url && $el.data('text') == text;
    });
}

function firstNonPinnedElement() {
    return $('#history .hist-elem:not(.pinned)').first();    
}

function insertHistoryItemBefore($item, $before) {
    if ($before.length) {
        $item.insertBefore($before);
    } else {
        $("#history").append($item);
    }
}

function updateHistory(url, lcolor, rcolor, text) {
    var pinned = false, $insertBefore;
    var $existingEl = findHistoryElement(url, text);
    if ($existingEl.length) {
        pinned = $existingEl.data('pinned');
        $insertBefore = $existingEl.next();
        $existingEl.remove();
    } else {
        $insertBefore = firstNonPinnedElement();
        $('#history .hist-elem:nth-child(' + MAX_HISTORY_SIZE + ')').remove();
    }
    var item = { url: url, lcolor: lcolor, rcolor: rcolor, text: text, pinned: pinned };
    var $item = createHistoryElement(item);
    insertHistoryItemBefore($item, $insertBefore);
    saveHistory();
    $("#main-input").val('').focus();
}

function mapColor(color) {
    return COLORMAP[color] || color;
}

function buildOffThing(span_class) {
    var thing = $('<span class="glyphicon glyphicon-ban-circle">');
    var sr_span = $('<span class="sr-only">').text('off');
    return $('<span class="' + span_class + '">').append(thing).append(sr_span);
}

function buildColorSwatch(color, span_class) {
    var swatch = $('<span class="color-sel" style="background-color: ' + mapColor(color) + ';">');
    var sr_span = $('<span class="sr-only">').text(color);
    return $('<span class="' + span_class + '">').append(swatch).append(sr_span);
}

function buildColorItem(color, span_class) {
    if (color == 'off') {
        return buildOffThing(span_class);
    } else {
        return buildColorSwatch(color, span_class);
    }
}

function buildMenuItem(item) {
    return buildColorItem(item.value, 'color-menu-item');
}

function buildColorPicker(name, defaultColor) {
    var sel = 'select[name="' + name + '"]';
    $(sel).selectmenu({
        width: 65,
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

function sendRequest(url, lcolor, rcolor, text) {
    var data = { text: text };
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
    $.ajax(url, { method: 'POST', data: data }).done(function(response) {
        updateHistory(url, lcolor, rcolor, response);
    });
}

function formSubmit(event) {
    event.preventDefault();
    var lcolor = $('#color_select').val();
    var rcolor = $('#color2_select').val();
    var url = $('#tts_radio').is(':checked') ? '/say' : '/play';
    var text = $('#main-input').val();
    return sendRequest(url, lcolor, rcolor, text);
}

function moveItemBelowPinned($item) {
    $item.detach();
    insertHistoryItemBefore($item, firstNonPinnedElement());
}

function pinHistoryItem($item) {
    $item.data('pinned', true).addClass('pinned');
    $item.find('.pin-button').removeClass('btn-default').addClass('btn-primary');
    moveItemBelowPinned($item);
}

function unpinHistoryItem($item) {
    $item.data('pinned', false).removeClass('pinned');
    $item.find('.pin-button').removeClass('btn-primary').addClass('btn-default');
    moveItemBelowPinned($item);
}

function togglePinHistoryItem(event) {
    var $item = $(event.target).closest(".hist-elem");
    var pinned = $item.data('pinned');
    if (pinned) {
        unpinHistoryItem($item);
    } else {
        pinHistoryItem($item);
    }
    saveHistory();
}

function playHistoryItem(event) {
    var $item = $(event.target).closest(".hist-elem");
    $item.find('.play-button').css('opacity', '0.5');
    var item = parseHistoryElement($item);
    sendRequest(item.url, item.lcolor, item.rcolor, item.text);
}

$(document).ready(function() {
    buildColorPicker('color', 'cyan');
    buildColorPicker('color2', 'purple');
    $('#main-input').focus();
    $("#main-form").on("submit", formSubmit);
    $("#history").on("click", ".pin-button", togglePinHistoryItem);
    $("#history").on("click", ".play-button", playHistoryItem);
    loadHistory();
});