

module Laplacians

    using CUDA

    export compute_∇²_1d!, compute_∇²_2d!


    # CUDA kernels

    function ∇²_kernel!(∇²ϕ, ϕ, dx, N)
        i = threadIdx().x
        if i >= 2 && i <= (N-1)
            ∇²ϕ[i] = (ϕ[i-1] - 2*ϕ[i] + ϕ[i+1]) / dx^2
        end
        return
    end

    function ∇²_kernel_2d!(∇²ϕ, ϕ, dr, N, M)
        i = (blockIdx().x - 1) * blockDim().x + threadIdx().x
        j = (blockIdx().y - 1) * blockDim().y + threadIdx().y

        if i >= 2 && i <= (N-1) && j >= 2 && j <= (M-1)
            ∇²ϕ[i, j] = (ϕ[i-1, j] + ϕ[i+1, j] + ϕ[i, j-1] + ϕ[i, j+1] - 4ϕ[i,j]) / dr^2
        end
        return
    end


    # Define the callers

    function compute_∇²_1d!(∇²ϕ::CuArray{Float64}, ϕ::CuArray{Float64}, dx::Float64)
        N = length(ϕ)
        @cuda threads=N ∇²_kernel!(∇²ϕ, ϕ, dx, N)
    end

    function compute_∇²_2d!(∇²ϕ::CuArray{Float64}, ϕ::CuArray{Float64,2}, dr::Float64)
        N, M = size(ϕ)
        threads_per_block = (16, 16)
        blocks = (div(N, threads_per_block[1]) + 1, div(M, threads_per_block[2]) + 1)

        @cuda blocks=blocks threads=threads_per_block ∇²_kernel_2d!(∇²ϕ, ϕ, dr, N, M)

    end

end