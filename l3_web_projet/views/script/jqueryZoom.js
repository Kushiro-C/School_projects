$(document).ready(function () {

    var options = {
        width: 250,
        height: 350,
        zoomWidth: 500,
        offset: { vertical: 0, horizontal: 10 },
        zoomStyle: 'z-index:1',
        scale: 1.5
    }
    $("[zoomOn]").each(function (e) {
        new ImageZoom(document.getElementById($(this).attr('zoomOn')), options);
    });
});