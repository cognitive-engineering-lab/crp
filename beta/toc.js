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
        this.innerHTML = '<ol class="chapter"><li class="chapter-item affix "><a href="title-page.html">C++ to Rust Phrasebook</a></li><li class="chapter-item affix "><li class="part-title">Idioms</li><li class="chapter-item "><a href="idioms/constructors.html"><strong aria-hidden="true">1.</strong> Constructors</a><a class="toggle"><div>❱</div></a></li><li><ol class="section"><li class="chapter-item "><a href="idioms/constructors/default_constructors.html"><strong aria-hidden="true">1.1.</strong> Default constructors</a></li><li class="chapter-item "><a href="idioms/constructors/copy_and_move_constructors.html"><strong aria-hidden="true">1.2.</strong> Copy and move constructors</a></li><li class="chapter-item "><a href="idioms/constructors/rule_of_three_five_zero.html"><strong aria-hidden="true">1.3.</strong> Rule of three/five/zero</a></li></ol></li><li class="chapter-item "><a href="idioms/destructors.html"><strong aria-hidden="true">2.</strong> Destructors and resource cleanup</a></li><li class="chapter-item "><a href="idioms/data_modeling.html"><strong aria-hidden="true">3.</strong> Data modeling</a><a class="toggle"><div>❱</div></a></li><li><ol class="section"><li class="chapter-item "><a href="idioms/data_modeling/abstract_classes.html"><strong aria-hidden="true">3.1.</strong> Abstract classes, interfaces, and dynamic dispatch</a></li><li class="chapter-item "><a href="idioms/data_modeling/concepts.html"><strong aria-hidden="true">3.2.</strong> Concepts, interfaces, and static dispatch</a></li><li class="chapter-item "><a href="idioms/data_modeling/enums.html"><strong aria-hidden="true">3.3.</strong> Enums</a></li><li class="chapter-item "><a href="idioms/data_modeling/tagged_unions.html"><strong aria-hidden="true">3.4.</strong> Tagged unions and std::variant</a></li><li class="chapter-item "><a href="idioms/data_modeling/inheritance_and_reuse.html"><strong aria-hidden="true">3.5.</strong> Inheritance and implementation reuse</a></li><li class="chapter-item "><a href="idioms/data_modeling/templates.html"><strong aria-hidden="true">3.6.</strong> Template classes, functions, and methods</a></li><li class="chapter-item "><a href="idioms/data_modeling/template_specialization.html"><strong aria-hidden="true">3.7.</strong> Template specialization</a></li></ol></li><li class="chapter-item "><a href="idioms/null.html"><strong aria-hidden="true">4.</strong> Null (nullptr)</a><a class="toggle"><div>❱</div></a></li><li><ol class="section"><li class="chapter-item "><a href="idioms/null/sentinel_values.html"><strong aria-hidden="true">4.1.</strong> Sentinel values</a></li><li class="chapter-item "><a href="idioms/null/moved_members.html"><strong aria-hidden="true">4.2.</strong> Moved members</a></li><li class="chapter-item "><a href="idioms/null/zero_length_arrays.html"><strong aria-hidden="true">4.3.</strong> Zero-length arrays</a></li></ol></li><li class="chapter-item "><a href="idioms/encapsulation.html"><strong aria-hidden="true">5.</strong> Encapsulation</a><a class="toggle"><div>❱</div></a></li><li><ol class="section"><li class="chapter-item "><a href="idioms/encapsulation/headers.html"><strong aria-hidden="true">5.1.</strong> Header files</a></li><li class="chapter-item "><a href="idioms/encapsulation/anonymous_namespaces.html"><strong aria-hidden="true">5.2.</strong> Anonymous namespaces and static</a></li><li class="chapter-item "><a href="idioms/encapsulation/private_and_friends.html"><strong aria-hidden="true">5.3.</strong> Private members and friends</a></li><li class="chapter-item "><a href="idioms/encapsulation/private_constructors.html"><strong aria-hidden="true">5.4.</strong> Private constructors</a></li></ol></li><li class="chapter-item "><a href="idioms/type_equivalents.html"><strong aria-hidden="true">6.</strong> Type equivalents</a></li><li class="chapter-item "><a href="idioms/promotions_and_conversions.html"><strong aria-hidden="true">7.</strong> Type promotions and conversions</a></li><li class="chapter-item "><a href="idioms/user-defined_conversions.html"><strong aria-hidden="true">8.</strong> User-defined conversions</a></li><li class="chapter-item "><a href="idioms/overloading.html"><strong aria-hidden="true">9.</strong> Overloading</a></li><li class="chapter-item "><a href="idioms/out_params.html"><strong aria-hidden="true">10.</strong> Out parameters</a><a class="toggle"><div>❱</div></a></li><li><ol class="section"><li class="chapter-item "><a href="idioms/out_params/multiple_return.html"><strong aria-hidden="true">10.1.</strong> Multiple return values</a></li><li class="chapter-item "><a href="idioms/out_params/optional_return.html"><strong aria-hidden="true">10.2.</strong> Optional return values</a></li><li class="chapter-item "><a href="idioms/out_params/pre-allocated_buffers.html"><strong aria-hidden="true">10.3.</strong> Pre-allocated buffers</a></li></ol></li><li class="chapter-item "><li class="part-title">Patterns</li><li class="chapter-item "><a href="patterns/crtp.html"><strong aria-hidden="true">11.</strong> Curiously recurring template pattern (CRTP)</a></li></ol>';
        // Set the current, active page, and reveal it if it's hidden
        let current_page = document.location.href.toString().split("#")[0].split("?")[0];
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
