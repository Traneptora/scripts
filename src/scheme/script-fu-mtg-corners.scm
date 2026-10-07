(define (script-fu-mtg-corners filename)
    (let*
        (
            (theImage (car (file-png-load #:run-mode RUN-NONINTERACTIVE #:file filename)))
            (theLayer (vector-ref (car (gimp-image-get-layers theImage)) 0))
        )
        (gimp-layer-add-alpha theLayer)
        (gimp-selection-all theImage)
        (gimp-selection-shrink theImage (quotient (car (gimp-image-get-height theImage)) 58))
        (gimp-selection-invert theImage)
        (gimp-drawable-append-new-filter theLayer "gegl:color-to-alpha" "Color to Alpha" LAYER-MODE-REPLACE 1.0 #:transparency-threshold 0.5 #:opacity-threshold 0.5 #:color "white")
        (gimp-drawable-merge-filters theLayer)
        (gimp-selection-none theImage)
        (file-png-export #:run-mode RUN-NONINTERACTIVE #:image theImage #:file filename #:include-thumbnail FALSE #:include-color-profile TRUE #:compression 7)
        (gimp-image-delete theImage)
    )
)

(script-fu-register
    "script-fu-mtg-corners"                     ;function name
    "Delete MTG Corner Rounding"                ;menu label
    "Replaces the white corners created by MSE with rounded transparent corners."        ;description
    "Leo Izen (Traneptora)"                     ;author
    "No Copyright. Public domain. CC0 in jurisdictions without public domain."   ;copyright notice
    "22 March 2025"                             ;date created
    ""                                          ;image type that the script works on
    SF-FILENAME "Filename" ""
)
(script-fu-menu-register "script-fu-mtg-corners" "<Image>/File/Create")
