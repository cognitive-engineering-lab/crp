// Populate the sidebar
//
// This is a script, and not included directly in the page, to control the total size of the book.
// The TOC contains an entry for each page, so if each page includes a copy of the TOC,
// the total size of the page becomes O(n**2).
class MDBookSidebarScrollbox extends HTMLElement {
    constructor() {
        super();
    }
    connectedCallback() {
        this.innerHTML = '<ol class="chapter"><li class="chapter-item expanded affix "><a href="how_to_use_this_book.html">How to use this book</a></li><li class="chapter-item expanded affix "><li class="part-title">Idioms</li><li class="chapter-item expanded "><a href="idioms/data_modeling.html"><strong aria-hidden="true">1.</strong> Data modeling</a></li><li><ol class="section"><li class="chapter-item expanded "><a href="idioms/data_modeling/enums.html"><strong aria-hidden="true">1.1.</strong> Enums</a></li><li class="chapter-item expanded "><a href="idioms/data_modeling/tagged_unions.html"><strong aria-hidden="true">1.2.</strong> Tagged unions and std::variant</a></li><li class="chapter-item expanded "><a href="idioms/data_modeling/pure_virtual_classes.html"><strong aria-hidden="true">1.3.</strong> Pure virtual classes, interfaces, and dynamic dispatch</a></li><li class="chapter-item expanded "><a href="idioms/data_modeling/concepts.html"><strong aria-hidden="true">1.4.</strong> Concepts, interfaces, and static dispatch</a></li><li class="chapter-item expanded "><a href="idioms/data_modeling/inheritance_and_reuse.html"><strong aria-hidden="true">1.5.</strong> Inheritance and implementation reuse</a></li><li class="chapter-item expanded "><div><strong aria-hidden="true">1.6.</strong> Modeling shallow type hierarchies</div></li><li class="chapter-item expanded "><a href="idioms/data_modeling/templates.html"><strong aria-hidden="true">1.7.</strong> Template classes, functions, and methods</a></li><li class="chapter-item expanded "><div><strong aria-hidden="true">1.8.</strong> Template specialization</div></li><li class="chapter-item expanded "><div><strong aria-hidden="true">1.9.</strong> Static and dynamic dispatch (virtual methods)</div></li></ol></li><li class="chapter-item expanded "><div><strong aria-hidden="true">2.</strong> Null (nullptr)</div></li><li><ol class="section"><li class="chapter-item expanded "><a href="idioms/null/zero_length_arrays.html"><strong aria-hidden="true">2.1.</strong> Zero-length arrays</a></li><li class="chapter-item expanded "><div><strong aria-hidden="true">2.2.</strong> Sentinel values</div></li><li class="chapter-item expanded "><a href="idioms/null/moved_members.html"><strong aria-hidden="true">2.3.</strong> Moved members</a></li></ol></li><li class="chapter-item expanded "><div><strong aria-hidden="true">3.</strong> Encapsulation</div></li><li><ol class="section"><li class="chapter-item expanded "><a href="idioms/encapsulation/headers.html"><strong aria-hidden="true">3.1.</strong> Header files</a></li><li class="chapter-item expanded "><a href="idioms/encapsulation/anonymous_namespaces.html"><strong aria-hidden="true">3.2.</strong> Anonymous namespaces and static</a></li><li class="chapter-item expanded "><div><strong aria-hidden="true">3.3.</strong> Private members and friends</div></li><li class="chapter-item expanded "><div><strong aria-hidden="true">3.4.</strong> Returning const references</div></li></ol></li><li class="chapter-item expanded "><div><strong aria-hidden="true">4.</strong> Primitive types: int, float, size_t, etc.</div></li><li class="chapter-item expanded "><div><strong aria-hidden="true">5.</strong> Overloading</div></li><li class="chapter-item expanded "><a href="idioms/constructors.html"><strong aria-hidden="true">6.</strong> Constructors</a></li><li><ol class="section"><li class="chapter-item expanded "><a href="idioms/constructors/default_constructors.html"><strong aria-hidden="true">6.1.</strong> Default constructors</a></li><li class="chapter-item expanded "><a href="idioms/constructors/copy_and_move_constructors.html"><strong aria-hidden="true">6.2.</strong> Copy and move constructors</a></li><li class="chapter-item expanded "><a href="idioms/constructors/rule_of_three_five_zero.html"><strong aria-hidden="true">6.3.</strong> Rule of three/five/zero</a></li><li class="chapter-item expanded "><a href="idioms/constructors/partial_initialzation.html"><strong aria-hidden="true">6.4.</strong> Separate construction and initialization</a></li></ol></li><li class="chapter-item expanded "><a href="idioms/destructors.html"><strong aria-hidden="true">7.</strong> Destructors and resource cleanup</a></li><li class="chapter-item expanded "><div><strong aria-hidden="true">8.</strong> RTTI</div></li><li class="chapter-item expanded "><div><strong aria-hidden="true">9.</strong> Iterators</div></li><li class="chapter-item expanded "><div><strong aria-hidden="true">10.</strong> Out parameters</div></li><li><ol class="section"><li class="chapter-item expanded "><a href="idioms/out_params/multiple_return.html"><strong aria-hidden="true">10.1.</strong> Multiple return values</a></li><li class="chapter-item expanded "><a href="idioms/out_params/optional_return.html"><strong aria-hidden="true">10.2.</strong> Optional return values</a></li><li class="chapter-item expanded "><a href="idioms/out_params/pre-allocated_buffers.html"><strong aria-hidden="true">10.3.</strong> Pre-allocated buffers</a></li></ol></li><li class="chapter-item expanded "><a href="idioms/exceptions.html"><strong aria-hidden="true">11.</strong> Exceptions and error handling</a></li><li class="chapter-item expanded "><div><strong aria-hidden="true">12.</strong> Function objects, lambdas, and closures</div></li><li class="chapter-item expanded "><a href="idioms/object_identity.html"><strong aria-hidden="true">13.</strong> Object identity</a></li><li class="chapter-item expanded "><div><strong aria-hidden="true">14.</strong> Varargs</div></li><li class="chapter-item expanded "><div><strong aria-hidden="true">15.</strong> Attributes</div></li><li class="chapter-item expanded "><div><strong aria-hidden="true">16.</strong> STL type equivalents</div></li><li class="chapter-item expanded "><div><strong aria-hidden="true">17.</strong> Scratch buffers</div></li><li class="chapter-item expanded affix "><li class="part-title">Patterns</li><li class="chapter-item expanded "><div><strong aria-hidden="true">18.</strong> Visitor pattern and double dispatch</div></li><li class="chapter-item expanded "><div><strong aria-hidden="true">19.</strong> Pointer-to-implementation (PImpl)</div></li><li class="chapter-item expanded "><div><strong aria-hidden="true">20.</strong> Curiously recurring template pattern (CRTP)</div></li><li class="chapter-item expanded "><div><strong aria-hidden="true">21.</strong> X macros</div></li><li class="chapter-item expanded affix "><li class="part-title">Libraries</li><li class="chapter-item expanded affix "><li class="part-title">Optimizations</li><li class="chapter-item expanded "><div><strong aria-hidden="true">22.</strong> NRVO, RVO, and placement new</div></li><li class="chapter-item expanded affix "><li class="part-title">Tooling</li><li class="chapter-item expanded "><div><strong aria-hidden="true">23.</strong> Build systems (CMake)</div></li><li class="chapter-item expanded "><div><strong aria-hidden="true">24.</strong> Unit tests</div></li></ol>';
        // Set the current, active page, and reveal it if it's hidden
        let current_page = document.location.href.toString().split("#")[0];
        if (current_page.endsWith("/")) {
            current_page += "index.html";
        }
        var links = Array.prototype.slice.call(this.querySelectorAll("a"));
        var l = links.length;
        for (var i = 0; i < l; ++i) {
            var link = links[i];
            var href = link.getAttribute("href");
            if (href && !href.startsWith("#") && !/^(?:[a-z+]+:)?\/\//.test(href)) {
                link.href = path_to_root + href;
            }
            // The "index" page is supposed to alias the first chapter in the book.
            if (link.href === current_page || (i === 0 && path_to_root === "" && current_page.endsWith("/index.html"))) {
                link.classList.add("active");
                var parent = link.parentElement;
                if (parent && parent.classList.contains("chapter-item")) {
                    parent.classList.add("expanded");
                }
                while (parent) {
                    if (parent.tagName === "LI" && parent.previousElementSibling) {
                        if (parent.previousElementSibling.classList.contains("chapter-item")) {
                            parent.previousElementSibling.classList.add("expanded");
                        }
                    }
                    parent = parent.parentElement;
                }
            }
        }
        // Track and set sidebar scroll position
        this.addEventListener('click', function(e) {
            if (e.target.tagName === 'A') {
                sessionStorage.setItem('sidebar-scroll', this.scrollTop);
            }
        }, { passive: true });
        var sidebarScrollTop = sessionStorage.getItem('sidebar-scroll');
        sessionStorage.removeItem('sidebar-scroll');
        if (sidebarScrollTop) {
            // preserve sidebar scroll position when navigating via links within sidebar
            this.scrollTop = sidebarScrollTop;
        } else {
            // scroll sidebar to current active section when navigating via "next/previous chapter" buttons
            var activeSection = document.querySelector('#sidebar .active');
            if (activeSection) {
                activeSection.scrollIntoView({ block: 'center' });
            }
        }
        // Toggle buttons
        var sidebarAnchorToggles = document.querySelectorAll('#sidebar a.toggle');
        function toggleSection(ev) {
            ev.currentTarget.parentElement.classList.toggle('expanded');
        }
        Array.from(sidebarAnchorToggles).forEach(function (el) {
            el.addEventListener('click', toggleSection);
        });
    }
}
window.customElements.define("mdbook-sidebar-scrollbox", MDBookSidebarScrollbox);
