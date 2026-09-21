// Capitalise the first letter, for an admonition's title.
function titlecase(text) {
  if (!text) return '';
  const s = String(text);
  return s.charAt(0).toUpperCase() + s.slice(1);
}
