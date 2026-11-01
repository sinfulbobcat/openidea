export interface CppBridgeResponse {
  ok: boolean;
  value?: unknown;
  error?: { message?: string };
}

export interface CppNativeBridge {
  invoke(requestId: string, command: string, args: Record<string, unknown>): void;
  response: {
    connect(handler: (requestId: string, result: CppBridgeResponse) => void): void;
    disconnect(handler: (requestId: string, result: CppBridgeResponse) => void): void;
  };
}

declare global {
  interface Window {
    openideaNativeReady?: Promise<CppNativeBridge>;
  }
}

let nextRequestId = 0;

export function isCppHost(): boolean {
  return typeof window !== "undefined" && window.openideaNativeReady !== undefined;
}

export async function invokeCpp<T>(command: string, args: Record<string, unknown> = {}): Promise<T> {
  if (typeof window === "undefined" || !window.openideaNativeReady) {
    throw new Error("C++ host bridge is unavailable");
  }

  const bridge = await window.openideaNativeReady;
  const requestId = `${Date.now()}-${++nextRequestId}`;

  return new Promise<T>((resolve, reject) => {
    const handleResponse = (responseId: string, result: CppBridgeResponse) => {
      if (responseId !== requestId) return;
      bridge.response.disconnect(handleResponse);
      if (result.ok) resolve(result.value as T);
      else reject(new Error(result.error?.message ?? `C++ backend command failed: ${command}`));
    };

    bridge.response.connect(handleResponse);
    try {
      bridge.invoke(requestId, command, args);
    } catch (error) {
      bridge.response.disconnect(handleResponse);
      reject(error);
    }
  });
}
