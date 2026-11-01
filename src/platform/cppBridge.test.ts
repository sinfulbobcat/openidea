import { afterEach, describe, expect, it, vi } from "vitest";
import type { CppBridgeResponse, CppNativeBridge } from "./cppBridge";
import { invokeCpp, isCppHost } from "./cppBridge";

function mockBridge(response: CppBridgeResponse) {
  const handlers = new Set<(requestId: string, result: CppBridgeResponse) => void>();
  const invoke = vi.fn((requestId: string) => {
    queueMicrotask(() => handlers.forEach((handler) => handler(requestId, response)));
  });
  const bridge: CppNativeBridge = {
    invoke,
    response: {
      connect: (handler) => handlers.add(handler),
      disconnect: (handler) => handlers.delete(handler),
    },
  };
  return { bridge, invoke, handlers };
}

afterEach(() => {
  delete window.openideaNativeReady;
});

describe("C++ host bridge", () => {
  it("detects a Qt host and resolves command responses", async () => {
    const { bridge, invoke, handlers } = mockBridge({ ok: true, value: { platform: "windows" } });
    window.openideaNativeReady = Promise.resolve(bridge);

    expect(isCppHost()).toBe(true);
    await expect(invokeCpp("app_info")).resolves.toEqual({ platform: "windows" });
    expect(invoke).toHaveBeenCalledWith(expect.any(String), "app_info", {});
    expect(handlers.size).toBe(0);
  });

  it("rejects native command errors with their message", async () => {
    window.openideaNativeReady = Promise.resolve(
      mockBridge({
        ok: false,
        error: { message: "command unavailable" },
      }).bridge,
    );

    await expect(invokeCpp("open_project")).rejects.toThrow("command unavailable");
  });

  it("fails clearly when no C++ bridge is installed", async () => {
    expect(isCppHost()).toBe(false);
    await expect(invokeCpp("app_info")).rejects.toThrow("C++ host bridge is unavailable");
  });
});
