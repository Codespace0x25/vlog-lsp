;; .comfig.el --- Project-specific config for vlog language dev

(setq compilation-auto-jump-to-first-error t)

;; Compile command using Projectile root
(defun call/MakeFile-test ()
  "Run 'make -j3 run' from the Projectile project root."
  (interactive)
  (if-let ((project-root (projectile-project-root)))
      (let ((default-directory project-root))
        (compile "make -j3 run"))
    (message "Not in a Projectile project.")))

;; Bind to C-c m t
(global-set-key (kbd "C-c m t") #'call/MakeFile-test)


;; 🧠 Dev LSP server registration for vlog-mode (only runs locally)
(require 'lsp-mode)

(add-to-list 'lsp-language-id-configuration '(vlog-mode . "vlog"))

(let* ((project-root (or (projectile-project-root) default-directory))
       (lsp-binary (expand-file-name "build/vlog-lsp-dev" project-root)))
  (when (and (file-exists-p lsp-binary) (executable-find lsp-binary))
    (message "🧪 Registering project-local LSP server: %s" lsp-binary)
    (lsp-register-client
     (make-lsp-client
      :new-connection (lsp-stdio-connection (list lsp-binary))
      :major-modes '(vlog-mode)
      :server-id 'vlog-local-dev
      :priority -1
      :multi-root t
      :initialized-fn (lambda (workspace)
                        (message "✅ vlog-local-dev initialized"))))
    (add-hook 'vlog-mode-hook #'lsp)))

;; Optional: Log LSP messages to *lsp-log*
(setq lsp-log-io t)
