---
title: API Reference - mp-units
template: api_reference.html
hide:
  - navigation
  - toc
---

<!-- markdownlint-disable MD033 -->
<!-- markdownlint-disable MD013 -->

<script src="../javascripts/jquery.min.js"></script>
<script src="../javascripts/iframeResizer.min.js"></script>

<iframe id="apiIframe" style="width: 100%; border: none;"></iframe>

<script>
// injects scripts to the iframe source code
$('#apiIframe').on('load', function() {
  body = $(this).contents().find('body');
  body.append('<scr' + 'ipt src="../../javascripts/iframeResizer.contentWindow.min.js"></scr' + 'ipt>');
  body.append('<scr' + 'ipt>' +
    'var links = document.querySelectorAll("a");' +
    'for (var i = 0; i < links.length; i++) {' +
    '  links[i].addEventListener("click", whichLinkWasClicked);' +
    '}' +
    '' +
    'function whichLinkWasClicked(evt) {' +
    '  window.parentIFrame.sendMessage(evt.target.href);' +
    '  evt.preventDefault();' +
    '}' +
  '</scr' + 'ipt>');
});

// reloads the window with the content when a user navigates over the browser's history
function locationHashChanged() {
  window.location.reload();
}
window.onhashchange = locationHashChanged;

apiIframe = document.getElementById('apiIframe');
hash = window.location.hash;
if (hash.length == 0) {
  // sets default hash for the API Reference
  hash = "#index.html";
  history.pushState(null, "", hash);
}

// set the iframe source based on the hash in the URL
apiIframe.src = "gen/" +  hash.slice(1);

// receives content height from the subpage displayed in the iframe
// works only for the pages in the same domain as the main docs
iFrameResize({
  scrolling: "yes",

  // obtains the link URL clicked in the subpage
  onMessage: function(messageData) {
    url = messageData.message;
    if (url.search("api_reference/gen") == -1) {
      window.open(url);
    }
    else {
      pos_start = messageData.message.lastIndexOf('/');
      hash = '#' + messageData.message.slice(pos_start + 1)
      history.pushState(null, "", hash);
      window.location.reload();
    }
  }
},'#apiIframe')
</script>
