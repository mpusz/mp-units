// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// Transform marked code blocks into Compiler Explorer (CE) iframes at runtime.
// Usage in Markdown: place a directive as the FIRST non-empty line inside a fenced code block.
//
// ```cpp
// // ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3"
// #include <mp-units/systems/si.h>
// ...
// ```
//
// Optional parameters on the directive:
//  - height: number (default 650)
//  - compiler: string (default clang2110)
//  - flags: string (default "-std=c++23 -stdlib=libc++ -O3")
(function (root, factory) {
  const api = factory();
  if (typeof module === 'object' && module.exports) {
    // Node/CommonJS: export only (used by tests)
    module.exports = api;
  } else {
    // Browser: attach and bootstrap
    root.ceEmbed = api;
    if (typeof root !== 'undefined') {
      try { api.bootstrap(); } catch (e) { /* noop */ }
    }
  }
})(typeof globalThis !== 'undefined' ? globalThis : this, function () {
  function encodeCode(code) {
    // Strict percent-encode all except: a-z, A-Z, 0-9, '-', '_', '.', '~', '/', ':', ',', '(', ')'
    // Space -> '+'; Newline -> '%0A'; Single quote -> !'
    // This matches CE's encoding for share links.
    const safe = /[a-zA-Z0-9\-_.~/:,()]/;
    let out = '';
    for (let i = 0; i < code.length; ++i) {
      const ch = code[i];
      if (ch === ' ') {
        out += '+';
      } else if (ch === '\n') {
        out += '%0A';
      } else if (ch === "'") {
        out += "!'";
      } else if (safe.test(ch)) {
        out += ch;
      } else {
        // Use encodeURIComponent for all other chars (including &)
        out += encodeURIComponent(ch);
      }
    }
    return out;
  }

  function buildSrc(code, compiler, flags) {
    // Normalize line endings and trim one trailing newline to match CE share output
    let norm = code.replace(/\r\n/g, '\n');
    if (norm.endsWith('\n')) norm = norm.slice(0, -1);
    // Replace all single '!' with '!!' for CE link encoding, as required
    norm = norm.replace(/!/g, '!!');
    const CODE = encodeCode(norm)
      // Prefer unencoded slashes to better match CE's own sharing format
      .replace(/%2F/g, '/');
    const FLAGS = encodeURIComponent(flags).replace(/%20/g, '+');
    // Layout closely matching a known-good CE embed
    return (
      "https://godbolt.org/e?hideEditorToolbars=true#g:!((g:!((g:!((h:codeEditor,i:(" +
      "filename:'1',fontScale:15,fontUsePx:'0',j:1,lang:c%2B%2B,selection:(endColumn:1,endLineNumber:1,positionColumn:1,positionLineNumber:1,selectionStartColumn:1,selectionStartLineNumber:1,startColumn:1,startLineNumber:1),source:'" + CODE + "'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),k:100,l:'4',m:65.44554455445545,n:'0',o:'',s:0,t:'0'),(g:!((h:executor,i:(argsPanelShown:'1',compilationPanelShown:'1',compiler:" + compiler + ",compilerName:'',compilerOutShown:'0',execArgs:'',execStdin:'',fontScale:16,fontUsePx:'0',j:1,lang:c%2B%2B,libs:!((name:mp-units,ver:trunk)),options:'" + FLAGS + "',overrides:!(),runtimeTools:!(),source:1,stdinPanelShown:'1',wrap:'0'),l:'5',n:'0',o:'Executor+x86-64+clang+21.1.0+(C%2B%2B,+Editor+%231)',t:'0')),header:(),l:'4',m:34.554455445544555,n:'0',o:'',s:0,t:'0')),l:'3',n:'0',o:'',t:'0')),version:4"
    );
  }

  function parseInfo(info) {
    // Parse key=value pairs where value may be quoted and contain spaces.
    // Supports: key=value | key="value with spaces" | key='value with spaces'
    const meta = { height: 650, compiler: 'clang2110', flags: '-std=c++23 -stdlib=libc++ -O3' };
    const regex = /(\w+)=((?:"[^"]*")|(?:'[^']*')|[^\s]+)/g;
    let match;
    while ((match = regex.exec(info)) !== null) {
      const key = match[1];
      let val = match[2];
      if ((val.startsWith('"') && val.endsWith('"')) || (val.startsWith("'") && val.endsWith("'"))) {
        val = val.slice(1, -1);
      }
      if (key === 'height') meta.height = parseInt(val, 10) || meta.height;
      else if (key === 'compiler') meta.compiler = val;
      else if (key === 'flags') meta.flags = val;
    }
    return meta;
  }

  function transform() {
    // Find fenced code blocks rendered by MkDocs Material
    const pres = document.querySelectorAll('pre code');
    let transformed = 0;
    pres.forEach((codeEl) => {
      const raw = codeEl.textContent.replace(/^\uFEFF/, ''); // strip BOM if present
      const lines = raw.split('\n');
      // Find first non-empty line
      let idx = 0;
      while (idx < lines.length && lines[idx].trim() === '') idx++;
      const firstLine = (lines[idx] || '').trimStart();
      if (!firstLine.startsWith('// ce-embed')) return;

      // Parse attributes after marker
      const attr = firstLine.replace(/^\/\/\s*ce-embed\s*/, '');
      const meta = parseInfo('ce-embed ' + attr);
      const code = lines.slice(idx + 1).join('\n');
      const src = buildSrc(code, meta.compiler, meta.flags);

      const iframe = document.createElement('iframe');
      iframe.setAttribute('src', src);
      iframe.setAttribute('loading', 'lazy');
      iframe.setAttribute('title', 'Compiler Explorer - mp-units (Headers)');
      iframe.style.width = '100%';
      iframe.style.height = `${meta.height}px`;
      iframe.style.border = '1px solid #ccc';
      iframe.style.borderRadius = '6px';

      // Replace the containing highlight block if present, else the pre
      const highlight = codeEl.closest('div.highlight, pre, figure');
      if (highlight) {
        highlight.replaceWith(iframe);
        transformed++;
      }
    });
    if (transformed > 0 || window.localStorage.getItem('ceEmbedDebug') === '1') {
      // Lightweight debug line in console to confirm script ran
      // Enable verbose logging by running in console: localStorage.setItem('ceEmbedDebug','1')
      console.log(`[ce-embed] transformed blocks: ${transformed}`);
    }
  }

  function bootstrap() {
    // Only run in browser environments with a DOM
    if (typeof window === 'undefined' || typeof document === 'undefined') return;
    // Hook into MkDocs Material page lifecycle
    if (window.document$ && typeof window.document$.subscribe === 'function') {
      // Run immediately for the initial page load
      try { transform(); } catch (e) { console.error('[ce-embed] initial transform error', e); }
      // And again on each SPA navigation
      window.document$.subscribe(() => transform());
    } else if (document.readyState === 'loading') {
      document.addEventListener('DOMContentLoaded', transform);
    } else {
      transform();
    }
  }

  return { encodeCode, buildSrc, parseInfo, transform, bootstrap };
});
