---
hide:
  - navigation
  - toc
---

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
  hash = "#full";
  history.pushState(null, "", hash);
}

// set the iframe source based on the hash in the URL
if (hash.startsWith("#wg21.link/")) {
  apiIframe.src = "https://" +  hash.slice(1);
  apiIframe.height = 900;
}
else
  apiIframe.src = "gen/" +  hash.slice(1) + ".html";

// receives content height from the subpage displayed in the iframe
// works only for the pages in the same domain as the main docs
iFrameResize({
  scrolling: "yes",

  // obtains the link URL clicked in the subpage
  onMessage: function(messageData) {
    url = messageData.message;
    if (url.startsWith("https://wg21.link/")) {
      hash = '#' + messageData.message.replace("https://", "");
    }
    else {
      pos = messageData.message.indexOf('#');
      if(pos == -1) {
        pos = messageData.message.lastIndexOf('/');
      }
      hash = '#' + messageData.message.slice(pos + 1);
    }
    history.pushState(null, "", hash);
    window.location.reload();
  }
},'#apiIframe')
</script>
