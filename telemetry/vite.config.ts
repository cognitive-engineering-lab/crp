import * as cp from "node:child_process";
import fs from "node:fs";
import { resolve } from "node:path";
import { defineConfig } from "vite";

let commitHash = cp.execSync("git rev-parse HEAD").toString("utf-8").trim();
let branch = cp
  .execSync("git rev-parse --abbrev-ref HEAD")
  .toString("utf-8")
  .trim();

let manifest = JSON.parse(fs.readFileSync("package.json", "utf-8"));
export default defineConfig(({ mode }) => ({
  build: {
    lib: {
      entry: resolve(__dirname, "src/main.ts"),
      name: "Telemetry",
      formats: ["iife"]
    },
    rollupOptions: {
      external: Object.keys(manifest.dependencies || {})
    }
  },
  define: {
    "process.env.NODE_ENV": JSON.stringify(mode),
    COMMIT_HASH: JSON.stringify(commitHash),
    BRANCH: JSON.stringify(branch),
    TELEMETRY_URL: JSON.stringify("https://rust-book.willcrichton.net/logs")
  },
  test: {
    environment: "jsdom",
    deps: {
      inline: [/^(?!.*vitest).*$/]
    }
  }
}));
