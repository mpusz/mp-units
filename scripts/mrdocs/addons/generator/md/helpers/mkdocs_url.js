// Turn a MrDocs page path into the URL MkDocs serves it at.
//
// With `use_directory_urls` (the default) MkDocs serves `a/b.md` from `a/b/`,
// one level deeper than the source file, while `index.md` is served from its
// own directory. MrDocs hands templates absolute page paths such as
// `/mp_units/quantity-01.md`, so a link has to be computed against where the
// *current* page is served rather than where its file sits.
//
// Doing it here rather than post-processing the output keeps the knowledge in
// one place and means MkDocs never has to rewrite links inside raw HTML, which
// it does not parse.
function mkdocs_url(href, from) {
  if (!href) return '';
  href = String(href);
  // Leave anything that is not a local page alone.
  if (/^[a-z][a-z0-9+.-]*:/i.test(href)) return href;

  const hash = href.indexOf('#');
  const anchor = hash >= 0 ? href.slice(hash) : '';
  const target = hash >= 0 ? href.slice(0, hash) : href;
  if (!/\.md$/i.test(target)) return href;

  const served = function(path) {
    const parts = String(path).replace(/^\/+/, '').split('/');
    const last = parts.pop();
    if (last.toLowerCase() !== 'index.md') {
      parts.push(last.replace(/\.md$/i, ''));
    }
    return parts.filter(function(p) {
      return p.length > 0;
    });
  };

  const to = served(target);
  const here = served(from || '/index.md');

  // `here` is the directory the current page is served from; walk up to the
  // common prefix, then down into the target.
  let common = 0;
  while (common < here.length && common < to.length &&
         here[common] === to[common]) {
    common += 1;
  }
  const up = here.length - common;
  const parts = [];
  for (let i = 0; i < up; ++i) parts.push('..');
  for (let i = common; i < to.length; ++i) parts.push(to[i]);

  if (parts.length === 0) return './' + anchor;
  return parts.join('/') + '/' + anchor;
}
