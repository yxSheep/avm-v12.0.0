#!/bin/bash -l
#SBATCH --job-name=inter_qp235_A2
#SBATCH --array=0-21               # 22 个任务: 0 到 21（对应 22 个 .y4m 文件）
#SBATCH --cpus-per-task=1
#SBATCH --output=logs_inter_qp235_A2/%A_%a.out
#SBATCH --partition=fat8t          # 根据你的集群调整

# ==================== 配置区 ====================
USER_HOME=/gpfs/hpc/home/g/xinyang
SIF=${USER_HOME}/ubuntu2404-avm12.sif
WRAPPER=/gpfs/hpc/software/apptainer/run-apptainer

INPUT_DIR="${USER_HOME}/ivcworkspace/data/A2"
OUTPUT_DIR="${USER_HOME}/ivcworkspace/result-v12-dev-inter"
LOG_DIR="${OUTPUT_DIR}/log_qp235"
OBU_DIR="${OUTPUT_DIR}/obu_qp235"

QP=235

# 获取所有 .y4m 文件列表（按字母顺序，确保一致性）
mapfile -t Y4M_FILES < <(find "$INPUT_DIR" -name "*.y4m" | sort)

TOTAL_VIDEOS=${#Y4M_FILES[@]}
if [ $SLURM_ARRAY_TASK_ID -ge $TOTAL_VIDEOS ]; then
    echo "Task ID $SLURM_ARRAY_TASK_ID exceeds total videos ($TOTAL_VIDEOS). Exiting."
    exit 0
fi

# 创建输出和日志目录
mkdir -p "$OUTPUT_DIR" "$LOG_DIR" "$OBU_DIR"

# 获取当前任务对应的视频文件
CURRENT_FILE="${Y4M_FILES[$SLURM_ARRAY_TASK_ID]}"
BASE_NAME=$(basename "$CURRENT_FILE" .y4m)

echo "Task ${SLURM_ARRAY_TASK_ID}: processing video '${BASE_NAME}' (${CURRENT_FILE})"

# 设置 Apptainer 临时目录（避免冲突）
export APPTAINER_SESSIONDIR=/tmp/${USER}/apptainer_sess_${SLURM_JOB_ID}_${SLURM_ARRAY_TASK_ID}
export APPTAINER_TMPDIR=/tmp/${USER}/apptainer_tmp_${SLURM_JOB_ID}_${SLURM_ARRAY_TASK_ID}
export APPTAINER_CACHEDIR=/tmp/${USER}/apptainer_cache_${SLURM_JOB_ID}_${SLURM_ARRAY_TASK_ID}
mkdir -p "$APPTAINER_SESSIONDIR" "$APPTAINER_TMPDIR" "$APPTAINER_CACHEDIR"

$WRAPPER exec \
    --bind "$USER_HOME:/gpfs/hpc/home/g/xinyang" \
    "$SIF" \
    /gpfs/hpc/home/g/xinyang/ivcworkspace/code/avm-v12-dev-inter/build/aomenc \
    --verbose \
    --codec=av1 \
    -v \
    --psnr \
    --obu \
    --frame-parallel=0 \
    --cpu-used=1 \
    --limit=65 \
    --skip=0 \
    --passes=1 \
    --end-usage=q \
    --i420 \
    --use-fixed-qp-offsets=1 \
    --deltaq-mode=0 \
    --enable-tpl-model=0 \
    --bit-depth=10 \
    --qp="$QP" \
    --tile-rows=0 \
    --tile-columns=0 \
    --threads=1 \
    --row-mt=0 \
    --enable-fwd-kf=0 \
    --enable-keyframe-filtering=0 \
    --enable-intrabc-ext=2 \
    --min-gf-interval=16 \
    --max-gf-interval=16 \
    --gf-min-pyr-height=4 \
    --gf-max-pyr-height=4 \
    --kf-min-dist=65 \
    --kf-max-dist=65 \
    --lag-in-frames=19 \
    --auto-alt-ref=1 \
    --enable-deblocking=1 \
    --enable-cdef=1 \
    --enable-ccso=1 \
    --enable-restoration=1 \
    --enable-gdf=1 \
    -o "$OBU_DIR/${BASE_NAME}.obu" \
    "/gpfs/hpc/home/g/xinyang/ivcworkspace/data/A2/${BASE_NAME}.y4m" \
    > "$LOG_DIR/${BASE_NAME}.log" 2>&1

echo "Task ${SLURM_ARRAY_TASK_ID} finished: ${BASE_NAME}"
